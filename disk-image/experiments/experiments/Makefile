obj-m += hello-1.o 
 
PWD := $(CURDIR) 
 
all: 
	make -C ../../../../linux-v4.19.83/ M=$(PWD) modules 
 
clean: 
	make -C ../../../../linux-v4.19.83/ M=$(PWD) clean