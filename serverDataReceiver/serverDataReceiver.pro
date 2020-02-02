
QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serverDataReceiver
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

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_NO_WARNING_OUTPUT
#DEFINES += QT_NO_DEBUG_OUTPUT


SOURCES += \
        src/all_struct_parser/abstractp.cpp \
        src/all_struct_parser/unistructparser.cpp \
        src/averagetime.cpp \
        src/main.cpp \
        src/restarter.cpp \
        src/tcpserver.cpp \
        src/widget.cpp

HEADERS += \
        src/all_struct_parser/abstractp.h \
        src/all_struct_parser/unistructparser.h \
        src/averagetime.h \
        src/dataheader.h \
        src/headerdescription.h \
        src/headerdescription_impl.h \
        src/parsers/abstractparser_impl.h \
        src/parsers/abstractparsersignalsslots.h \
        src/parsers/jsonparser_impl.h \
        src/parsers/structparser_impl.h \
        src/parsersmanager_impl.h \
        src/somestruct.h \
        src/parsers/abstractparser.h \
        src/parsers/jsonparser.h \
        src/parsers/structparser.h \
        src/parsersmanager.h \
        src/restarter.h \
        src/tcpserver.h \
        src/widget.h


