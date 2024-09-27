#!/bin/sh
PATH=/C/msys64/home/zeng/gateway/LowConfGateway/gui/GZ248/qmqtt/qtmqtt-5.12/bin:/C/msys64/mingw64/bin:$PATH
export PATH
QT_PLUGIN_PATH=/C/msys64/mingw64/share/qt5/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec "$@"
