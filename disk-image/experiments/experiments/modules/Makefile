# https://sysprog21.github.io/lkmpg/
obj-m += hello-1.o 
 
PWD := $(CURDIR) 
# INCLUDES := -I$(PWD)/../include
# ccflags-y += $(INCLUDES)
 
all: 
	make -C $(HOME)/linux-5.4-rc8/ M=$(PWD) modules
 
clean: 
	make -C $(HOME)/linux-5.4-rc8/ M=$(PWD) clean