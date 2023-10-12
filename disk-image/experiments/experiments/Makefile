CC := gcc
AS := as
LD := ld

ASFLAGS :=
CFLAGS := -Iinclude -g

SRCS_C := $(wildcard src/*.c)
SRCS_S := $(wildcard src/*.S)
SRCS := $(SRCS_C) $(SRCS_S)

# $(info $(SRCS))

OBJECTS_C := $(patsubst src/%.c, obj/%.o, $(SRCS_C))
OBJECTS_S := $(patsubst src/%.S, obj/%.o, $(SRCS_S))
OBJECTS := $(OBJECTS_C) $(OBJECTS_S)

# $(info $(OBJECTS))

TARGETS_C := $(patsubst src/%.c, bin/%, $(SRCS_C))
TARGETS_S := $(patsubst src/%.S, bin/%, $(SRCS_S))
TARGETS := $(TARGETS_C) $(TARGETS_S)

# $(info $(TARGETS))

# shd spectre lab

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf obj $(TARGETS)

obj/%.o: src/%.S
	@echo " AS    $<"
	@mkdir -p obj
	@$(AS) -c $(ASFLAGS) $< -o $@

obj/%.o: src/%.c
	@echo " CC    $<"
	@mkdir -p obj
	@$(CC) -c $(CFLAGS) $< -o $@

bin/%: obj/%.o
	@echo " LD    $@"
	@mkdir -p obj bin
	@$(CC) -o $@ $^

# https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o : %.c
%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))