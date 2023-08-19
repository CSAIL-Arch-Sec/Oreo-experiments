# https://sysprog21.github.io/lkmpg/
obj-m += hello-1.o 
 
PWD := $(CURDIR) 
INCLUDES := -I$(PWD)/../include

ccflags-y += $(INCLUDES)
 
all: 
	make -C $(HOME)/protect-kaslr/linux-v4.19.83/ M=$(PWD) modules 
 
clean: 
	make -C $(HOME)/protect-kaslr/linux-v4.19.83/ M=$(PWD) clean