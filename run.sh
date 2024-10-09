ln -s /home/zeng/share/5.22esp32/esp-idf espsdk

# cd /home/zeng/share/5.22esp32/esp-idf; . ./export.sh ; cd -;sudo chmod 777 /dev/ttyCH343USB0;  idf.py -p /dev/ttyCH343USB0 flash monitor
# cd /home/zeng/share/5.22esp32/esp-idf; . ./export.sh ; cd -;sudo chmod 777 /dev/ttyCH341USB0;idf.py -p /dev/ttyCH341USB0 flash monitor
cd /home/zeng/share/esp32/esp-idf/;  . ./export.sh ; cd -;sudo chmod 777 /dev/ttyCH343USB0;  idf.py -p /dev/ttyCH343USB0 flash monitor

# /home/zeng/.espressif/./tools/xtensa-esp-elf/esp-14.2.0_20240906/xtensa-esp-elf/bin/xtensa-esp32-elf-addr2line -pfiaC -e /home/zeng/share/gateway/LowConfGateway/build/hello_world.elf 0x4000c354

# script minicom.log #保存日志
