
工具的网盘地址如下：
https://pan.baidu.com/s/1dDtssIt#list/p … 1%E4%BA%AB
找到arm-eabi-工具下载包
gcc-linaro-5.3.1-2016.05-x86_64_arm-eabi.tar.xz
下载后放置到需要编译的位置，进行解压缩
解压分为两步，.tar.xz格式-->.tar格式
xz -d xxxx.tar.xz
.tar-->解压出文件夹
tar xvf xxxx.tar
至此出现了编译工具的文件夹gcc-linaro-5.3.1-2016.05-x86_64_arm-eabi

xboot编译命令：

make CROSS_COMPILE=/home/alan123/Downloads/gcc-linaro-5.3.1-2016.05-x86_64_arm-eabi/bin/arm-eabi- PLATFORM=arm32-f1c100s

烧录到spi flash命令
sudo ./sunxi-fel -p spiflash-write 0 /mnt/hgfs/xboot/output/xboot.bin;sudo ./sunxi-fel exec 0

或者如下烧录指令
sudo ./sunxi-fel -p spiflash-write 0 /mnt/hgfs/xboot/output/xboot.bin

此次备忘一下，怕自己忘记了

编译make流程在14.04和16.04失败了，此前成功过，用此命令，备注。