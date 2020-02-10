TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wfatal-errors

HEADERS += \
    headers/earth.h \
    headers/geometry.h \
    headers/logs.h \
    headers/parseNMEA.h \
    headers/position.h \
    headers/types.h

SOURCES += \
    src/earth.cpp \
    src/geometry.cpp \
    src/logs.cpp \
    src/parseNMEA.cpp \
    src/position.cpp \
    src/nmea-tests.cpp

INCLUDEPATH += headers/

TARGET = $$_PRO_FILE_PWD_/execs/nmea-tests

LIBS += -lboost_unit_test_framework
