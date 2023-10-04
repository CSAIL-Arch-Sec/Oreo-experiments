# See: https://tldp.org/LDP/lkmpg/2.6/html/x181.html
obj-m += labspectrekm.o

# Shared inc directory with userspace code for common IPC struct definitions
INCLUDES := \
	-I$(PWD)/../inc

# Some warnings we don't care about :)
IGNORE_WARNINGS := \
	-Wno-frame-larger-than \
	-Wno-char-subscripts \
	-Wno-unused-variable \

EXTRA_CFLAGS += $(INCLUDES) $(IGNORE_WARNINGS)
EXTRA_LDFLAGS += $(IGNORE_WARNINGS)

all:
	make -C $(HOME)/linux/ M=$(PWD) modules modules
# make -C /home/jzha/protect-kaslr/linux/ M=$(PWD) modules modules
clean:
	make -C $(HOME)/linux/ M=$(PWD) modules clean
# make -C /home/jzha/protect-kaslr/linux/ M=$(PWD) clean