TEMPLATE = app
CONFIG   += console
QT = core \
    network \
    xml
QT  +=sql
CONFIG(debug, debug|release) {
    DESTDIR = build/debug/bin
    OBJECTS_DIR = build/debug
    MOC_DIR = build/debug
    UI_DIR = build/debug
    RCC_DIR = build/debug
} else {
    DESTDIR = build/release/bin
    OBJECTS_DIR = build/release
    MOC_DIR = build/release
    UI_DIR = build/release
    RCC_DIR = build/release
}
SOURCES += src/main.cpp \
    src/ModbusFrame.cpp \
    src/ModbusServer.cpp \
    src/Server.cpp
HEADERS += src/ModbusFrame.h \
    src/ModbusServer.h \
    src/Server.h

RESOURCES += \
    resources/resource.qrc
