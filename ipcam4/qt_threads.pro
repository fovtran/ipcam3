# QT+=core
QT+=widgets network
# QT+=multimedia testlib

TEMPLATE = app
CONFIG   -= console testcase
CONFIG   += app_bundle thread c++17 windeployqt
#strict_c++ embed_translations static
DEFINES  +=
CONFIG += debug_and_release

release: DESTDIR = release
debug:   DESTDIR = debug
CONFIG(debug, debug|release) {
    #TARGET = debug_binary
	DESTDIR = $$OUT_PWD/debug
} else {
    #TARGET = release_binary
	DESTDIR = $$OUT_PWD/release
}

unix:UI_DIR = /tmp
unix:MOC_DIR = /tmp
unix:OBJECTS_DIR = /tmp
win32:UI_DIR = /tmp
win32:MOC_DIR = /tmp
win32:OBJECTS_DIR = /tmp
linux:MOC_DIR = /tmp
linux:UI_DIR = /tmp
linux:OBJECTS_DIR = /tmp

# QMAKE_CXX = clang++-14
QMAKE_CXX = g++
DEFINES += USE_MY_STUFF

win32:INCLUDEPATH += "../src/includes"
unix:INCLUDEPATH += "../src/includes"

HEADERS = ../src/includes/*.h
SOURCES = ../src/*.cpp

win32|unix {}

win32 {
	QMAKE_LFLAGS_WINDOWS +=	-fopenmp
}

unix {
	QMAKE_CXXFLAGS +=
}
linux {
	QMAKE_CXXFLAGS += -std=c++2a -march=athlon64-sse3 -msse3 -msse2 -msse -mmmx -mpopcnt
}

EXTERNAL_PROJECTS += ../library

run.commands += $$DESTDIR/program.exe
QMAKE_TARGETS += install

config.path = $$DESTDIR/config/
config.files = ../src/config/*.xml
resources.path = $$DESTDIR/resources/
resources.files = ../src/resources/*
INSTALLS += config resources

DISTFILES += \
	../data/atomicwhile_interlocking_types.qmodel \
	../data/programmodel.qmodel \
    ../data/Interlocked-Atomic-While.md

FORMS += \
	../src/resources/videocanvasform.ui
	
STATECHARTS += \
	../data/StateChart.scxml

RESOURCES += \
	../src/resources/resources.qrc
