#-------------------------------------------------
#
# Project created by QtCreator 2016-03-16T20:21:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = src
TEMPLATE = app
INCLUDEPATH += C:\\OpenCV\\build\\install\\include
LIBS += -LC:\\OpenCV\\build\\install\\x86\\mingw\\bin \
        libopencv_core2413 \
        libopencv_highgui2413 \
        libopencv_imgproc2413

SOURCES += main.cpp\
            mainwindow.cpp\
            defineddirection.cpp\
            laplace.cpp\
            canny.cpp \
            pointandline.cpp \
            gradientmethod.cpp \
            marrhildreth.cpp

HEADERS  += mainwindow.h\
            defineddirection.h\
            laplace.h\
            canny.h \
            pointandline.h \
            gradientmethod.h \
            marrhildreth.h

FORMS    += mainwindow.ui\
            defineddirection.ui\
            laplace.ui\
            canny.ui \
            pointandline.ui \
            gradientmethod.ui \
            marrhildreth.ui
