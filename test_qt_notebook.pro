#-------------------------------------------------
#
# Project created by QtCreator 2015-09-12T10:59:17
#
#-------------------------------------------------

QT       += core gui
QT	 += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_qt_notebook
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    customview.cpp \
    point.cpp \
    color.cpp \
    airodump_client.cpp \
    custombuttongroup.cpp \
    settings.cpp \
    widget.cpp \
    paletteview.cpp

HEADERS  += mainwindow.h \
    customview.h \
    point.h \
    color.h \
    airodump_client.h \
    custombuttongroup.h \
    settings.h \
    widget.h \
    paletteview.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/release/ -ltiff
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/ -ltiff
else:unix: LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/ -ltiff

INCLUDEPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../NetworkScan/libs/wifi/release/ -lairodump64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/..NetworkScan/libs/wifi/debug/ -lairodump64
else:unix: LIBS += -L$$PWD/../NetworkScan/libs/wifi/ -lairodump64

INCLUDEPATH += $$PWD/../NetworkScan/libs/wifi/include
DEPENDPATH += $$PWD/../NetworkScan/libs/wifi/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../NetworkScan/libs/wifi/release/libairodump64.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/..NetworkScan/libs/wifi/debug/libairodump64.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../NetworkScan/libs/wifi/release/airodump64.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../NetworkScan/libs/wifi/debug/airodump64.lib
else:unix: PRE_TARGETDEPS += $$PWD/../NetworkScan/libs/wifi/libairodump64.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/release/ -lpthread
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/ -lpthread
else:unix: LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -lpthread

INCLUDEPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/release/libpthread.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/libpthread.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/release/pthread.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/pthread.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/libpthread.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/release/ -lssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/ -lssl
else:unix: LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/ -lssl

INCLUDEPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/release/libssl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/libssl.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/release/ssl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/ssl.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/libssl.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/release/ -lcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/ -lcrypto
else:unix: LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/ -lcrypto

INCLUDEPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/release/libcrypto.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/libcrypto.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/release/crypto.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/crypto.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/lib/x86_64-linux-gnu/libcrypto.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/release/ -llapack
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/debug/ -llapack
else:unix: LIBS += -L$$PWD/../../../../usr/lib/ -llapack

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/release/liblapack.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/debug/liblapack.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/release/lapack.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/lib/debug/lapack.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/lib/liblapack.a

RESOURCES +=
