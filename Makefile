CONFIG_MODULE_SIG=n

MODULE ?= bmtest.o

obj-m += $(MODULE)

KBUILD ?= /lib/modules/$(shell uname -r)/build/

all:
	$(MAKE) -C $(KBUILD) M=$(PWD) modules

clean:
	$(MAKE) -C $(KBUILD) M=$(PWD) clean
