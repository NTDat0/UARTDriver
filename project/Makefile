obj-m += serdev_echo.o
KERNEL_DIR ?= $(HOME)/linux

all: module dt
	echo Builded Device Tree Overlay and kernel module

module:
	make -C $(KERNEL_DIR) \
		ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		M=$(PWD) modules
dt: serdev_overlay.dts
	dtc -@ -I dts -O dtb -o serdev_overlay.dtbo serdev_overlay.dts
clean:
	make -C $(KERNEL_DIR) \
		ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
		M=$(PWD) clean
	rm -rf serdev_overlay.dtbo
	
