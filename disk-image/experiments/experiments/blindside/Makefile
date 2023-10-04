CC := gcc
AS := as
LD := ld

OBJECTS_COMMON := main.o spectre_lab_helper.o

OBJECTS_PART2 := $(OBJECTS_COMMON) attacker-part2.o
TARGET_PART2  := part2

BUILD_OBJECTS_PART2 := $(patsubst %,build/%,$(OBJECTS_PART2))

TARGETS := $(TARGET_PART2)

ASFLAGS :=
CFLAGS := -Iinc -g -O0

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf build $(TARGETS)

build/%.o: src-common/%.s
	@echo " AS    $<"
	@mkdir -p build
	@$(AS) -c $(ASFLAGS) $< -o $@

build/%.o: src-common/%.c
	@echo " CC    $<"
	@mkdir -p build
	@$(CC) -c $(CFLAGS) $< -o $@

build/%.o: part2-src/%.c
	@echo " CC    $<"
	@mkdir -p build
	@$(CC) -c $(CFLAGS) $< -o $@

$(TARGET_PART2): $(BUILD_OBJECTS_PART2) Makefile
	@echo " LD    $@"
	@mkdir -p build
	@$(CC) -o $@ $(BUILD_OBJECTS_PART2)
