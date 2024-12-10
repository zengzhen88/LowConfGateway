ln -s /home/zeng/share/5.22esp32/esp-idf espsdk

cd /home/zeng/share/5.22esp32/esp-idf; . ./export.sh ; cd -;sudo chmod 777 /dev/ttyCH343USB0;  idf.py -p /dev/ttyCH343USB0 flash monitor
# cd /home/zeng/share/5.22esp32/esp-idf; . ./export.sh ; cd -;sudo chmod 777 /dev/ttyUSB0;  idf.py -p /dev/ttyUSB0 flash monitor
# cd /home/zeng/share/5.22esp32/esp-idf; . ./export.sh ; cd -;sudo chmod 777 /dev/ttyCH341USB0;idf.py -p /dev/ttyCH341USB0 flash monitor
# cd /home/zeng/share/esp32/esp-idf/;  . ./export.sh ; cd -;sudo chmod 777 /dev/ttyCH343USB0;  idf.py -p /dev/ttyCH343USB0 flash monitor

# /home/zeng/.espressif/./tools/xtensa-esp-elf/esp-14.2.0_20240906/xtensa-esp-elf/bin/xtensa-esp32-elf-addr2line -pfiaC -e /home/zeng/share/gateway/LowConfGateway/build/hello_world.elf 0x4000c354

# script minicom.log #保存日志



# uwb人员定位系统 github

# UWB（Ultra-Wideband）技术是一种无线通信技术，它可以实现毫米级的定位精度。UWB在身份验证、自动化控制和物联网等领域有着广泛的应用。

# 在GitHub上，有许多开源的UWB资源可供使用。以下是一些主要的UWB开源项目和系统：

    # uWB-for-Robotics: 这是一个针对移动机器人的UWB开源项目，它提供了用于定位和导航的软件库。

    # UWB-Access-Control: 这是一个基于UWB技术的访问控制系统，可以用于构建高精度空间的访问控制系统。

    # UWB-based-Indoor-Navigation: 这是一个基于UWB技术的室内导航系统，可以用于室内定位和导航。

    # UWB-Beacon-Simulator: 这是一个用于模拟UWB信号的仿真软件，可以用于测试和模拟UWB系统的性能。

    # UWB-Beacon-Design-Kit: 这是一个用于设计UWB信标的开源硬件设计工具包，可以用于制作自己的UWB定位设备。

    # UWB-Beacon-Protocols: 这是一个用于UWB信标通信协议的开源库，可以用于开发基于UWB的应用程序。

    # UWB-Beacon-Protocols-and-Tools: 这是一个用于UWB通信协议和工具的开源库，可以用于开发基于UWB的应用程序。

    # UWB-Beacon-Software-Defined-Radio: 这是一个用于UWB信标的开源软件无线电实现，可以用于构建自己的UWB系统。

    # UWB-Beacon-Toolkit: 这是一个用于UWB信标的开发工具包，提供了测试和调试UWB系统所需的工具。

    # UWB-Indoor-Positioning-System: 这是一个用于室内定位的UWB系统，可以用于构建高精度的室内定位系统。

# 在使用GitHub资源时，你需要遵循相关的许可协议，并遵循GitHub平台的使用规范。

# 以上提供的资源都可以在GitHub上找到，你可以通过在GitHub上搜索相关关键词（如“UWB”、“positioning system”等）来找到这些项目。

# 请注意，由于GitHub上的资源可能会随时更新或删除，因此建议在使用前检查最新的资源。
