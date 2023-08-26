## bochs-gdb

安装bochs-gdb， 安装目录需要重新创建一个，默认目录已经安装了一个
### 编译


```s    
./configure \
    --prefix=/usr/local/bochsgdb \
    --without-wx \
    --with-x11 \
    --with-x \
    --with-term \
    --disable-docbook \
    --enable-cpu-level=6 \
    --enable-fpu \
    --enable-3dnow \
    --enable-disasm \
    --enable-long-phy-address \
    --enable-disasm \
    --enable-pcidev \
    --enable-iodebug \
    --enable-usb \
    --enable-all-optimizations \
    --enable-gdb-stub \
    --with-nogui


    make

    sudo make install
```

然后我们还要做一件事：对这个新的bochs重命名，让它和之前的bochs区分。

我们只需要进入安装bochs-gdb的目录，然后执行以下命令
```s
cd /usr/local/bochsgdb/bin
sudo mv bochs bochs-gdb
sudo mv bximage bximage-gdb
```

修改环境变量
```s
gedit .bashrc

export PATH=/usr/local/bochsgdb/bin:$PATH

source ~/.bashrc
```


### configure报错

Curses library not found: tried curses, ncurses, termlib and pdcurses

安装
```shell

sudo apt-get install libncurses5-dev
```


这个参数--with-sdl， 添加后编译失败，没有解决
make过程中报错：fatal error: SDL.h: 没有那个文件或目录, 
```shell
#安装后没有解决
sudo apt-get install libsdl2-dev
```


编译需要开启iodebug参数，否则运行报错。提示没有iodebug

vscode配置

```shell
        {
            "name": "znix - Build and debug kernel",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/kernel.bin",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerServerAddress": "localhost:1234",  # 新增的
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Set Disassembly Flavor to Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ],
            "miDebuggerPath": "/usr/bin/gdb"  # 去掉preLaunchTask
        },
```