PWD :=$(PWD)
obj-m += i2c-bme280.o

CROSS := arm-linux-gnueabihf-

KERNEL := /home/linuxuser/raspi-build/

all:
	make -C $(KERNEL) ARCH=arm CROSS_COMPILE=$(CROSS) M=$(PWD) modules
clean:
	make -C $(KERNEL) SUBDIRS=$(PWD) clean

