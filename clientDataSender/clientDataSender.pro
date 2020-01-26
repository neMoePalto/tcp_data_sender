
QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clientDataSender
TEMPLATE = app

OBJECTS_DIR = temp/obj
MOC_DIR = temp/moc
DESTDIR = bin/
INCLUDEPATH += src/

# --- Compilation flags:
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++14
#CONFIG += C++14
#QMAKE_CXXFLAGS += -std=c++14


SOURCES += \
        src/averagetime.cpp \
        src/main.cpp \
        src/tcpsmartclient.cpp \
        src/widget.cpp

HEADERS += \
        src/averagetime.h \
        src/somestruct.h \
        src/tcpsmartclient.h \
        src/widget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    json_files/network.json \
    json_files/network_agent.json \
    json_files/some_struct.json
