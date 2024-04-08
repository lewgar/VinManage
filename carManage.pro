#-------------------------------------------------
#
# Project created by QtCreator 2024-02-18T20:43:38
#
#-------------------------------------------------

QT       += core gui sql
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = carManage
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

        OPENCV_LIB = $(MDWARE_HOME)"/common/third_party/opencv-3.4.1/x64/vc15/lib"
        OPENCV_INCLUDE = $(MDWARE_HOME)"/common/third_party/opencv-3.4.1/include"
DEMO_INCLUDE = "C:/Users/lew/Documents/carManage/Yolov5Lib/Yolov5Lib/include"
DEMO_LIB = "C:/Users/lew/Documents/carManage/Yolov5Lib/Yolov5Lib/lib"
MIDDLEWARE_OPENCV_LIB = "E:/Program Files (x86)/opencv-3.4.1/x64/vc15/lib"
MIDDLEWARE_OPENCV_INCLUDE = "E:/Program Files (x86)/opencv-3.4.1/include"
INCLUDEPATH += $${DEMO_INCLUDE}  $${MIDDLEWARE_OPENCV_INCLUDE} $${OPENCV_INCLUDE}
LIBS += -L$${DEMO_LIB} -L{MIDDLEWARE_OPENCV_LIB} -L$${OPENCV_LIB}
CONFIG(debug, debug|release) {
LIBS += -lopencv_world341d -lonnxruntime -lonnxruntime_providers_shared -lYOLOv5Detectd
}

CONFIG(release, debug|release) {
LIBS += -lopencv_world341 -lonnxruntime -lonnxruntime_providers_shared -lYOLOv5Detect
}

