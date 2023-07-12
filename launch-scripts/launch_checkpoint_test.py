import os
import sys
from uuid import UUID
from itertools import starmap
from itertools import product

from gem5art.artifact import Artifact
from gem5art.run import gem5Run
from gem5art.tasks.tasks import run_gem5_instance
import multiprocessing as mp

packer = Artifact.registerArtifact(
    command = '''
        wget https://releases.hashicorp.com/packer/{PACKER_VERSION}/packer_{PACKER_VERSION}_linux_amd64.zip;
        unzip packer_{PACKER_VERSION}_linux_amd64.zip;
        rm packer_{PACKER_VERSION}_linux_amd64.zip;
    '''.format(PACKER_VERSION="1.9.1"),
    typ = 'binary',
    name = 'packer',
    path =  'disk-image/packer',
    cwd = 'disk-image',
    documentation = 'program to build disk images'
)

experiments_repo = Artifact.registerArtifact(
    command = 'git clone git@github.com:jz268/protect-kaslr-experiments.git experiments',
    typ = 'git repo',
    name = 'experiments',
    path =  './',
    cwd = '../',
    documentation = 'protect kaslr experiments repo'
)

gem5_repo = Artifact.registerArtifact(
    command = 'git clone git@github.mit.edu:arch-sec/protect-kaslr-gem5.git gem5;',
    typ = 'git repo',
    name = 'gem5',
    path =  'gem5/',
    cwd = './',
    documentation = 'shixin gem5 repo'
)

m5_binary = Artifact.registerArtifact(
    command = 'scons build/x86/out/m5',
    typ = 'binary',
    name = 'm5',
    path =  'gem5/util/m5/build/x86/out/m5',
    cwd = 'gem5/util/m5',
    inputs = [gem5_repo,],
    documentation = 'm5 utility'
)

disk_image = Artifact.registerArtifact(
    command = './build.sh',
    typ = 'disk image',
    name = 'syscall',
    cwd = 'disk-image',
    path = 'disk-image/syscall/syscall-image/syscall',
    inputs = [packer, experiments_repo, m5_binary,],
    documentation = 'disk image using ubuntu 18.04 with benchmarks and evaluations installed?'
)

gem5_binary = Artifact.registerArtifact(
    command = 'scons build/X86/gem5.opt -j12',
    typ = 'gem5 binary',
    name = 'gem5',
    cwd = 'gem5/',
    path =  'gem5/build/X86/gem5.opt',
    inputs = [gem5_repo,],
    documentation = 'gem5 binary'
)

# linux_repo = Artifact.registerArtifact(
#     command = 'git clone --single-branch -b linux-5.4-rc8-protect-kaslr git@github.mit.edu:arch-sec/protect-kaslr-linux.git linux;',
#     typ = 'git repo',
#     name = 'linux',
#     path =  'linux/',
#     cwd = './',
#     documentation = 'shixin linux kernel repo'
# )

# using unmodified linux for now just to test

linux_repo = Artifact.registerArtifact(
    command = '''git clone --branch v4.19.83 --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;
    mv linux linux-stable''',
    typ = 'git repo',
    name = 'linux-stable',
    path =  'linux-stable/',
    cwd = './',
    documentation = 'linux kernel source code repo'
)

linux_binary = Artifact.registerArtifact(
    name = 'vmlinux-4.19.83',
    typ = 'kernel',
    path = 'linux-stable/vmlinux-4.19.83',
    cwd = 'linux-stable/',
    command = '''
    cp ../config.4.19.83 .config;
    make -j8;
    cp vmlinux vmlinux-4.19.83;
    ''',
    inputs = [experiments_repo, linux_repo,],
    documentation = "kernel binary for v4.19.83"
)

checkpoint = Artifact.registerArtifact(
    command = '../gem5/build/X86/gem5.opt gem5-configs/x86-save.py',
    typ = 'checkpoint',
    name = 'checkpoint',
    path =  'm5out/m5.cpt',
    cwd = './',
    inputs = [gem5_binary, m5_binary, disk_image,],
    documentation = 'checkpoint ??'
)

def createRun(bench, cpu):

    binary_gem5 = 'gem5/build/X86/gem5.opt'
    artifact_gem5 = gem5_binary

    return gem5Run.createFSRun(
        'testing thing',
        binary_gem5,
        'x86-restore.py',
        f'''results/run_/{bench}/{cpu}''',
        artifact_gem5, gem5_repo, experiments_repo,
        'linux-stable/vmlinux-4.19.83',
        'disk-image/syscall/syscall-image/syscall',
        linux_binary, disk_image,
        bench, cpu,
        timeout = 240*60*60 #240 hours
        )

if __name__ == "__main__":
    cpus = ['o3']
    benchmarks = ['test']
    
    def worker(run):
        run.run()
        json = run.dumpsJson()
        print(json)

    jobs = []

    # For the cross product of tests, create a run object.
    runs = starmap(createRun, product(benchmarks, cpus))
    # Run all of these experiments in parallel
    for run in runs:
        jobs.append(run)

    with mp.Pool(mp.cpu_count() // 2) as pool:
         pool.map(worker, jobs)