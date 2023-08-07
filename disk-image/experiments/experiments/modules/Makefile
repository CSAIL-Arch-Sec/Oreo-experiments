obj-m += hello-1.o 
 
PWD := $(CURDIR) 
 
all: 
	make -C $(HOME)/protect-kaslr/linux-v4.19.83/ M=$(PWD) modules 
 
clean: 
	make -C $(HOME)/protect-kaslr/linux-v4.19.83/ M=$(PWD) clean