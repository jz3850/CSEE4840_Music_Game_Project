ifneq (${KERNELRELEASE},)

# KERNELRELEASE defined: we are being compiled as part of the Kernel
        obj-m := vga_framebuffer.o audio.o

else

# We are being compiled as a module: use the Kernel build system

	KERNEL_SOURCE := /usr/src/linux-headers-$(shell uname -r)
        PWD := $(shell pwd)

default: module game

module:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules

hello: hello.c sprite.c color.c framebuffer.c fbputchar.c usbkeyboard.c audio_user.c Note.c
	${CC} -Wall -pthread -o hello hello.c sprite.c color.c framebuffer.c fbputchar.c usbkeyboard.c audio_user.c Note.c -lm -lusb-1.0

clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
	${RM} hello

TARFILES = Makefile README vga_framebuffer.h vga_framebuffer.c hello.c
TARFILE = lab3-sw.tar.gz
.PHONY : tar
tar : $(TARFILE)

$(TARFILE) : $(TARFILES)
	tar zcfC $(TARFILE) .. $(TARFILES:%=lab3-sw/%)

endif 
