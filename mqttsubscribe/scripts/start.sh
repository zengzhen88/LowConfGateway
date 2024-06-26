#!/bin/sh

echo "start()"

UDHCPCD_PID=$(ps | grep "/sbin/dhcpcd" | grep -v grep | awk '{print $1}')
if [ -n "$UDHCPCD_PID" ]; then
    kill $UDHCPCD_PID
fi

mount -t ubifs ubi0:miservice /config
mount -t ubifs ubi0:customer /customer
mount -t ubifs ubi0:appconfigs /appconfigs

insmod /config/modules/4.9.84/ehci-hcd.ko
insmod /config/modules/4.9.84/usb-storage.ko

# 中断引脚
echo 47 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio47/direction
echo rising > /sys/class/gpio/gpio47/edge

# 复位WiFi
echo 71 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio71/direction
echo 0 > /sys/class/gpio/gpio71/value
sleep 0.5
echo 1 > /sys/class/gpio/gpio71/value

# 网关状态
echo 14 >> /sys/class/gpio/export
echo out > /sys/class/gpio/gpio14/direction
echo 1 > /sys/class/gpio/gpio14/value

# 网关心跳
echo 13 >> /sys/class/gpio/export
echo out > /sys/class/gpio/gpio13/direction
echo 1 > /sys/class/gpio/gpio13/value

# 加载WiFi驱动
/config/wifi/ssw01bInit.sh

ifconfig wlan0 up

RUST_LOG=warn /root/jgd215/jgd215 > /root/jgd215/jgd215.log 2>&1 &
