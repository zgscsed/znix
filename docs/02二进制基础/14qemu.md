## qemu安装使用

### qemu安装

```
tar xvJf qemu-7.0.0.tar.xz
cd qemu-7.0.0
mkdir build && cd build
qemu-7.0.0/configure  # 检测依赖

make -j8
sudo make install

```

依赖
```
sudo apt-get install libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev indent bison flex pip
sudo apt-get install ninja-build
```


### qemu 执行

修改makefile文件，新增qemu， qemu-gdb启动
```
.PHONY: qemu
qemu: $(BUILD)/master.img
	qemu-system-i386 \
	-m 32M \
	-boot c \
	-hda $<

.PHONY: qemug
qemug: $(BUILD)/master.img
	qemu-system-i386 \
	-s -S \
	-m 32M \
	-boot c \
	-hda $<
```