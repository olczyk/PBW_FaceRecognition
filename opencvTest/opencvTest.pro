#-------------------------------------------------
#
# Project created by QtCreator 2014-10-26T22:58:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opencvTest
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

INCLUDEPATH += C://opencv-mingw//install//include
LIBS += -LC://opencv-mingw//bin \
    libopencv_contrib249 \
    libopencv_core249 \
    libopencv_features2d249 \
    libopencv_flann249 \
    libopencv_gpu249 \
    libopencv_highgui249 \
    libopencv_imgproc249 \
    libopencv_legacy249 \
    libopencv_ml249 \
    libopencv_nonfree249 \
    libopencv_objdetect249 \
    libopencv_ocl249 \
    libopencv_photo249 \
    libopencv_stitching249 \
    libopencv_superres249 \
    libopencv_video249 \
    libopencv_videostab249
