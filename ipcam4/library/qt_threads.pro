QT=core gui widgets network
# testlib multimedia
TARGET = somelib
TEMPLATE = lib
CONFIG   -= console testcase
CONFIG   += app_bundle thread c++17 windeployqt
CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    TARGET = debug_binary
	DESTDIR = $$OUT_PWD/build/debug
} else {
    TARGET = release_binary
	DESTDIR = $$OUT_PWD/build/release
}

unix:UI_DIR = /tmp
unix:MOC_DIR = /tmp
unix:OBJECTS_DIR = /tmp

QMAKE_CXX = clang++-16
DEFINES += USE_MY_STUFF

win32:INCLUDEPATH += "src/includes"
unix:INCLUDEPATH += "src/includes"

HEADERS = library/*.h
SOURCES = library/*.cpp

linux {
	QMAKE_CXXFLAGS += -std=c++2a -march=athlon64-sse3 -msse3 -msse2 -msse -mmmx -mpopcnt
}

QMAKE_TARGETS += install
