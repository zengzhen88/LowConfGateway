QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../main/common.c \
    ../../main/message.c \
    httpserver.cpp \
    httpserver/HttpExample.cpp \
    httpserver/WHttpServer.cpp \
    httpserver/WThreadPool.cpp \
    httpserver/mongoose.cpp \
    main.cpp \
    subwindow.cpp


HEADERS += \
    ../../main/common.h \
    ../../main/message.h \
    httpserver.h \
    httpserver/HttpExample.h \
    httpserver/IHttpServer.h \
    httpserver/LockQueue.hpp \
    httpserver/WHttpServer.h \
    httpserver/WThreadPool.h \
    httpserver/mongoose.h \
    subwindow.h

QMAKE_CXXFLAGS += -fpermissive 

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ini.qrc


DISTFILES += \
    httpserver/httpserver.pri

INCLUDEPATH += $$PWD/../../main/






win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdlib/pre_build/windows/lib/release/ -lssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdlib/pre_build/windows/lib/debug/ -lssl
INCLUDEPATH += $$PWD/../../thirdlib/pre_build/windows/inc
DEPENDPATH += $$PWD/../../thirdlib/pre_build/windows/inc

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/release/libssl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/debug/libssl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/release/ssl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/debug/ssl.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdlib/pre_build/windows/lib/release/ -lcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdlib/pre_build/windows/lib/debug/ -lcrypto

INCLUDEPATH += $$PWD/../../thirdlib/pre_build/windows/inc
DEPENDPATH += $$PWD/../../thirdlib/pre_build/windows/inc

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/release/libcrypto.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/debug/libcrypto.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/release/crypto.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/debug/crypto.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../thirdlib/pre_build/windows/lib/release/ -lcjson.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../thirdlib/pre_build/windows/lib/debug/ -lcjson.dll


INCLUDEPATH += $$PWD/../../thirdlib/pre_build/windows/inc
DEPENDPATH += $$PWD/../../thirdlib/pre_build/windows/inc

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/release/libcjson.dll.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/debug/libcjson.dll.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/release/libcjson.dll.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/windows/lib/debug/libcjson.dll.lib

unix:!macx: LIBS += -L$$PWD/../../thirdlib/pre_build/ubuntu/lib/release/ -lcjson

INCLUDEPATH += $$PWD/../../thirdlib/pre_build/ubuntu/inc
DEPENDPATH += $$PWD/../../thirdlib/pre_build/ubuntu/inc

unix:!macx: PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/ubuntu/lib/release/libcjson.a

unix:!macx: LIBS += -L$$PWD/../../thirdlib/pre_build/ubuntu/lib/release/ -lssl

INCLUDEPATH += $$PWD/../../thirdlib/pre_build/ubuntu/inc
DEPENDPATH += $$PWD/../../thirdlib/pre_build/ubuntu/inc

unix:!macx: PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/ubuntu/lib/release/libssl.a

unix:!macx: LIBS += -L$$PWD/../../thirdlib/pre_build/ubuntu/lib/release/ -lcrypto

INCLUDEPATH += $$PWD/../../thirdlib/pre_build/ubuntu/inc
DEPENDPATH += $$PWD/../../thirdlib/pre_build/ubuntu/inc

unix:!macx: PRE_TARGETDEPS += $$PWD/../../thirdlib/pre_build/ubuntu/lib/release/libcrypto.a

unix:!macx: LIBS += -L$$PWD/qmqtt/qtmqtt-5.12/linuxbuild/lib/ -lQt5Mqtt

INCLUDEPATH += $$PWD/qmqtt/qtmqtt-5.12/src/mqtt/
DEPENDPATH += $$PWD/qmqtt/qtmqtt-5.12/src/mqtt/
#window
#unix:!macx: PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/linuxbuild/lib/libQt5Mqtt.a -lpthread libwsock32 libws2_32 -lcrypt32

#linux
unix:!macx: PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/linuxbuild/lib/libQt5Mqtt.a -lpthread


win32: LIBS += -L$$PWD/qmqtt/qtmqtt-5.12/dllbuild/lib/ -lQt5Mqtt

INCLUDEPATH += $$PWD/qmqtt/qtmqtt-5.12/dllbuild/include
DEPENDPATH += $$PWD/qmqtt/qtmqtt-5.12/dllbuild/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/dllbuild/lib/Qt5Mqtt.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/dllbuild/lib/libQt5Mqtt.a
