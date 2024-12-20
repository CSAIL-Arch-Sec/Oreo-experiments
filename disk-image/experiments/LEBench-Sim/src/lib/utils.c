#define _GNU_SOURCE

#include <utils.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef DISABLE_TIMER
    #ifdef USE_RDTSCP
        // measure execution time with rdtsc/rdtscp
        // https://github.com/google/highwayhash/blob/master/highwayhash/tsc_timer.h
        void _start_timer(TimeType *ts) {
            *ts = _rdtsc_google_begin();
        }

        void _stop_timer(TimeType *ts) {
            *ts = _rdtscp_google_end();
        }

        double _get_duration(TimeType *start, TimeType *end) {
            return *end - *start; // in CPU cycles
        }
    #elif defined(USE_M5_RPNS)
        // measure execution time with m5 rpns
        void _start_timer(TimeType *ts) {
            *ts = m5_rpns();
        }

        void _stop_timer(TimeType *ts) {
            *ts = m5_rpns();
        }

        double _get_duration(TimeType *start, TimeType *end) {
            return *end - *start; // in CPU cycles
        }
    #else
        // measure execution time with Linux gettime
        void _start_timer(TimeType *ts) {
            clock_gettime(CLOCK_MONOTONIC, ts);
        }

        void _stop_timer(TimeType *ts) {
            clock_gettime(CLOCK_MONOTONIC, ts);
        }

        double _get_duration(TimeType *start, TimeType *end) {
            return get_timespec_diff_nsec(start, end); // in nano seconds
        }
    #endif // USE_RDTSCP
#else // DISABLE_TIMER
    // disable time measurement; it is useful if you are measuring the
    // CPI within ROIs in a simulator
    void _start_timer(TimeType *ts) { assert(false); }
    void _stop_timer(TimeType *ts) { assert(false); }
    double _get_duration(TimeType *start, TimeType *end) { assert(false); }
#endif // DISABLE_TIMER

double get_timespec_diff_sec(struct timespec *tstart, struct timespec *tend) {
    if (tstart && tend) {
        return (double)(tend->tv_sec - tstart->tv_sec) +
               (double)(tend->tv_nsec - tstart->tv_nsec) * 1e-9;
    } else {
        return 0.0;
    }
}

double get_timespec_diff_nsec(struct timespec *tstart, struct timespec *tend) {
    if (tstart && tend) {
        return (double)(tend->tv_sec - tstart->tv_sec) * 1e9 +
               (double)(tend->tv_nsec - tstart->tv_nsec);
    } else {
        return 0.0;
    }
}

// data processing related
#ifndef MAX_ITER
#define MAX_ITER 5
#endif

#ifndef TOLERANCE
#define TOLERANCE 3
#endif
void aggregate(double *data, size_t size, double *mean, double *stddev,
               double *max, double *min) {
    double avg = 0.0, dev = 0.0;
    double _avg = 0.0, _stddev = 0.0;

    bool changed = true;
    size_t iter = 0, removed = 0, valid_size = size;
    while (iter < MAX_ITER && changed) {
        *max = (double)LLONG_MIN;
        *min = (double)LLONG_MAX;
        changed = iter == 0;
        removed = 0;
        valid_size = size;
        avg = 0.0;
        dev = 0.0;

        for (size_t idx = 0; idx < size; idx++) {
            if (iter == 0 || fabs(data[idx] - _avg) < TOLERANCE * _stddev) {
                avg += data[idx];
                *max = data[idx] > *max ? data[idx] : *max;
                *min = data[idx] < *min ? data[idx] : *min;
            } else {
                changed = true;
                removed += 1;
            }
        }

        // account for the removed
        valid_size -= removed;
        if (valid_size) {
            avg /= (double) valid_size;
        } else {
            fprintf(stderr,
                    ZWARN "Too much noise! Stick to results from iteration: %lu\n",
                    iter - 1);
            avg = _avg;
            dev = _stddev * _stddev;
            break;
        }

        for (size_t idx = 0; idx < size; idx++) {
            if (iter == 0 || fabs(data[idx] - _avg) < TOLERANCE * _stddev) {
                dev += ((data[idx] - avg) / valid_size) * (data[idx] - avg);
            }
        }

        // save the results for next iteration
        _avg = avg;
        _stddev = sqrt(dev);

        if (_avg < 1e-3 || _stddev / _avg < 1e-3) {
            break;
        }

        iter++;
    }

    if (size != 0) {
        *mean = avg;
        if (size == 1) {
            *stddev = 0;
        } else {
            *stddev = sqrt(dev);
        }
    } else {
        *mean = NAN;
        *stddev = NAN;
        *max = NAN;
        *min = NAN;
    }
}

static int double_cmp(const void *a, const void *b) {
    if (*(double*)a < *(double*)b) return -1;
    else if (*(double*)a > *(double*)b) return 1;
    else return 0;
}

#ifndef IMPRECISION
#define IMPRECISION 0.01
#endif
double closest_k(double *data, size_t size, unsigned int k) {
    if (size == 0) return NAN;
    if (size == 1) return data[0];

    qsort(data, size, sizeof(double), double_cmp);
    size_t cont_cnt = 0, idx;
    for (idx = 1; idx < size; idx++) {
        if ((fabs(data[idx] - data[idx - 1]) / data[idx]) < IMPRECISION) {
            cont_cnt += 1;
            if (cont_cnt == k) {
                return data[idx - cont_cnt];
            }
        } else {
            cont_cnt = 0;
        }
    }

    if (cont_cnt != k) {
        fprintf(stderr, ZWARN"Only found %lu closest\n", cont_cnt);
    }
    return data[idx - cont_cnt];
}

#ifndef CLOSEST_K
#define CLOSEST_K 5
#endif
void collect_results(double *data, size_t size, BenchConfig* config,
                     BenchResult *res) {
#ifdef PRINT_RAW_DATA
    for (size_t idx = 0; idx < size; idx++) {
        printf("[Raw] %f\n", data[idx]);
    }
#endif

    res->config = config;
    res->child = NULL;
    res->errored = false;
#ifndef DISABLE_TIMER
    aggregate(data, size, &res->mean, &res->stddev, &res->max, &res->min);
    res->k_closest = closest_k(data, size, CLOSEST_K);
#endif
}

// common functions
int create_and_fill(char *filepath, size_t size, char c) {
    remove(filepath);
    char *buf = malloc(size);
    if (!buf) return -1;
    memset(buf, c, size);

    int fd = open(filepath, O_RDWR | O_CREAT, 0664);
    if (fd < 0) {
        fprintf(stderr, ZERROR "Failed to open file at %s\n", filepath);
        goto err;
    }
    UNUSED char _ = write(fd, buf, size);

err:
    free(buf);
    return fd;
}

double *init_diff_array(size_t iter_cnt) {
    double *diffs = (double *)malloc(sizeof(double) * iter_cnt);
    if (!diffs) {
        fprintf(stderr, ZERROR "Out of memory\n");
        return NULL;
    }

    for (size_t i = 0; i < iter_cnt; i++) {
        diffs[i] = 0.0;
    }
    return diffs;
}
