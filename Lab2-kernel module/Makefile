KERNELDIR=/lib/modules/$(shell uname -r)/build

#MODULES = file.ko
obj-m += file.o

all:
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
