ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m += sound/soc/bcm/
subdir-y += arch/arm/boot/dts/overlays

# Enable fixups to support overlays on BCM2835 platforms
ifeq ($(CONFIG_ARCH_BCM2835),y)
	DTC_FLAGS ?= -@ -H epapr
	dts-dirs += arch/arm/boot/dts/overlays
endif

else

# normal makefile
export KDIR ?= /lib/modules/`uname -r`/build
export INSTALL_MOD_DIR ?= kernel

build:
	$(MAKE) -C $(KDIR) M=$$PWD

install: build
	$(MAKE) -C $(KDIR) M=$$PWD modules_install
	cp -f arch/arm/boot/dts/overlays/tpa-hermes-rpi.dtbo /boot/firmware/overlays

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

endif
