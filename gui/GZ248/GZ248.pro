QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common.cpp \
    httpserver.cpp \
    httpserver/HttpExample.cpp \
    httpserver/WHttpServer.cpp \
    httpserver/WThreadPool.cpp \
    httpserver/mongoose.cpp \
    main.cpp \
    message.cpp \
    mqtt.cpp \
    mqttcollection.cpp \
    qnavigationwidget.cpp \
    subwindow.cpp

HEADERS += \
    httpserver.h \
    httpserver/HttpExample.h \
    httpserver/IHttpServer.h \
    httpserver/LockQueue.hpp \
    httpserver/WHttpServer.h \
    httpserver/WThreadPool.h \
    httpserver/mongoose.h \
    mqtt.h \
    mqttcollection.h \
    qnavigationwidget.h \
    common.h \
    message.h \
    subwindow.h



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ini.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/thirdlib/lib/release/ -lcjson
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/thirdlib/lib/debug/ -lcjson
else:unix: LIBS += -L$$PWD/thirdlib/lib/ -lcjson

INCLUDEPATH += $$PWD/thirdlib
DEPENDPATH += $$PWD/thirdlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/libcjson.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/libcjson.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/cjson.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/cjson.lib
else:unix: PRE_TARGETDEPS += $$PWD/thirdlib/lib/libcjson.a

DISTFILES += \
    httpserver/httpserver.pri


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/thirdlib/lib/release/ -lmosquitto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/thirdlib/lib/debug/ -lmosquitto
else:unix: LIBS += -L$$PWD/thirdlib/lib/ -lmosquitto

INCLUDEPATH += $$PWD/thirdlib
DEPENDPATH += $$PWD/thirdlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/libmosquitto.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/libmosquitto.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/mosquitto.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/mosquitto.lib
else:unix: PRE_TARGETDEPS += $$PWD/thirdlib/lib/libmosquitto.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/thirdlib/lib/release/ -lmosquittopp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/thirdlib/lib/debug/ -lmosquittopp
else:unix: LIBS += -L$$PWD/thirdlib/lib/ -lmosquittopp

INCLUDEPATH += $$PWD/thirdlib/inc/
DEPENDPATH += $$PWD/thirdlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/libmosquittopp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/libmosquittopp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/mosquittopp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/mosquittopp.lib
else:unix: PRE_TARGETDEPS += $$PWD/thirdlib/lib/libmosquittopp.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/thirdlib/lib/release/ -lssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/thirdlib/lib/debug/ -lssl
else:unix: LIBS += -L$$PWD/thirdlib/lib/ -lssl

INCLUDEPATH += $$PWD/thirdlib
DEPENDPATH += $$PWD/thirdlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/libssl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/libssl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/ssl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/ssl.lib
else:unix: PRE_TARGETDEPS += $$PWD/thirdlib/lib/libssl.a


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/thirdlib/lib/release/ -lcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/thirdlib/lib/debug/ -lcrypto
else:unix: LIBS += -L$$PWD/thirdlib/lib/ -lcrypto

INCLUDEPATH += $$PWD/thirdlib
DEPENDPATH += $$PWD/thirdlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/libcrypto.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/libcrypto.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/release/crypto.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/thirdlib/lib/debug/crypto.lib
else:unix: PRE_TARGETDEPS += $$PWD/thirdlib/lib/libcrypto.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/qmqtt/qtmqtt-5.12/build/lib/release/ -lQt5Mqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/qmqtt/qtmqtt-5.12/build/lib/debug/ -lQt5Mqtt
else:unix: LIBS += -L$$PWD/qmqtt/qtmqtt-5.12/build/lib/ -lQt5Mqtt

INCLUDEPATH += $$PWD/qmqtt/qtmqtt-5.12/build/include
DEPENDPATH += $$PWD/qmqtt/qtmqtt-5.12/build/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/build/lib/release/libQt5Mqtt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/build/lib/debug/libQt5Mqtt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/build/lib/release/Qt5Mqtt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/build/lib/debug/Qt5Mqtt.lib
else:unix: PRE_TARGETDEPS += $$PWD/qmqtt/qtmqtt-5.12/build/lib/libQt5Mqtt.a
