QT=core gui widgets network 
# testlib multimedia
TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle
DEFINES -= QT_NO_DEBUG
DEFINES  += QT_NO_SSL QT_DEBUG

unix:OBJECTS_DIR = ./tmp

LIBS += -LF:/usr/local/lib -lopencv_world
INCLUDEPATH += /usr/local/include/opencv4
#INCLUDEPATH += /DATA/SCRATCH2/TF4/CV/opencv-4.x/include
# DEPENDPATH += /DATA/SCRATCH2/TF4/CV/build/lib

QMAKE_CC = clang++-16
QMAKE_CXX = g++
#QMAKE_FLAGS += /MP /GF /favor /GL /LTCG /MT /Ox /EHa /w /Zp
QMAKE_CXXFLAGS += -O2 -msse3 -std=c++2a 
# -mpopcnt -mcx16 

#RESOURCES += qt_threads.qrc
HEADERS=MyThread.h
SOURCES=qt_threads.cpp MyThread.cpp

#DEPLOY_FLAGS = --no-compiler-runtime --no-webkit2 --no-angle --libdir .\release --plugindir .\release --release
#QMAKE_POST_LINK += windeployqt  $$DEPLOY_FLAGS .\release\qt_threads.exe
