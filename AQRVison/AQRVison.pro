#-------------------------------------------------
#
# Project created by QtCreator 2018-04-30T13:44:47
#
#-------------------------------------------------

QT += core gui
QT += network
QT += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AQRVison

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
           mainwindow.cpp \
           halfunc.cpp \
           dialogshapemodel.cpp \
           cam.cpp  \
    aqdata.cpp \
    aqlog.cpp \
    inifile.cpp \
    modbus_tcp_server.cpp \
    mythreads.cpp \
    dialogcalibration.cpp \
    dialogsetting.cpp \
    aidi_detect.cpp

HEADERS  += mainwindow.h \
            halfunc.h \
            dialogshapemodel.h \
            cam.h \
    aqdata.h \
    aqlog.h \
    inifile.h \
    modbus_tcp_server.h \
    mythreads.h \
    dialogcalibration.h \
    dialogsetting.h \
    base_struct.h \

FORMS    += mainwindow.ui \
            dialogshapemodel.ui \
    dialogcalibration.ui \
    dialogsetting.ui

INCLUDEPATH += "D:/Program Files/MVTec/HALCON-13.0/include/"

INCLUDEPATH += "D:/Program Files/Basler/Development/include/"

INCLUDEPATH += "E:/WorkingData/ProjectData/HongFu_Locate/ProjectCode/aidi_libs/334/aidi_vision334/include"

INCLUDEPATH += "E:/WorkingData/ProjectData/HongFu_Locate/ProjectCode/aidi_libs/334/jsoncpp/include"

INCLUDEPATH += "E:/WorkingData/ProjectData/HongFu_Locate/ProjectCode/aidi_libs/334/opencv3.3.0.0.1/include"

LIBS        += -L$$quote(D:\Program Files\MVTec\HALCON-13.0\lib\x64-win64) \
               -lhalcon \
               -lhalconcpp10 \
               -lhalconcpp

LIBS        += -L$$quote(D:\Program Files\Basler\Development\lib\x64) \

LIBS        += -L$$quote(E:\WorkingData\ProjectData\HongFu_Locate\ProjectCode\aidi_libs\334\opencv3.3.0.0.1\build\Release\x64) \
               -lopencv_ts300 \
               -lopencv_world300 \

LIBS        += -L$$quote(E:\WorkingData\ProjectData\HongFu_Locate\ProjectCode\aidi_libs\334\jsoncpp\build\Release\x64) \
               -ljson_cpp_dll

LIBS        += -L$$quote(E:\WorkingData\ProjectData\HongFu_Locate\ProjectCode\aidi_libs\334\aidi_vision334\Build\Release\x64) \
               -laidi_vision334

RESOURCES += aqlogo.qrc

QT += serialport serialbus

CONFIG += c++11
