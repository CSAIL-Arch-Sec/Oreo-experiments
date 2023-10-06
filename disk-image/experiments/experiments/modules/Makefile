# https://sysprog21.github.io/lkmpg/
obj-m += hello-1.o
obj-m += blindside-kernel.o
 
PWD := $(CURDIR) 
# INCLUDES := -I$(PWD)/../include
# ccflags-y += $(INCLUDES)
 
all: 
	make -C $(HOME)/linux/ M=$(PWD) modules
 
clean: 
	make -C $(HOME)/linux/ M=$(PWD) clean