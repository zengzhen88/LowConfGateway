#!/bin/sh
QT_VERSION=5.12.12
export QT_VERSION
QT_VER=5.12
export QT_VER
QT_VERSION_TAG=51212
export QT_VERSION_TAG
QT_INSTALL_DOCS=C:/msys64/mingw64/share/qt5/doc
export QT_INSTALL_DOCS
BUILDDIR=C:/msys64/home/zeng/gateway/LowConfGateway/gui/GZ248/qmqtt/qtmqtt-5.12/dllbuild/src/mqtt
export BUILDDIR
exec /C/msys64/mingw64/bin/qdoc.exe "$@"
