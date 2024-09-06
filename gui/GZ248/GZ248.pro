QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common.cpp \
    main.cpp \
    mainwindow.cpp \
    message.cpp \
    mqtt.cpp \
    qnavigationwidget.cpp \
    transmit.cpp

HEADERS += \
    mainwindow.h \
    mqtt.h \
    qnavigationwidget.h \
    common.h \
    message.h \
    transmit.h



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

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

DISTFILES +=

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

