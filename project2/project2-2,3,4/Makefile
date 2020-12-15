# at first type on ur terminal that $(uname -r) then u will get the version..
# that is using on ur system

obj-m += pid.o
obj-m += list_linear.o
obj-m += list_dfs.o
obj-m += list_traverse.o
obj-m += collatz.o

KDIR =/usr/src/linux-headers-$(shell uname -r)
 
all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
 
clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order
