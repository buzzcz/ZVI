#-------------------------------------------------
#
# Project created by QtCreator 2016-03-16T20:21:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = src
TEMPLATE = app

LIBS += -lopencv_core -lopencv_flann -lopencv_highgui -lopencv_imgproc

SOURCES += main.cpp\
        mainwindow.cpp\
        defineddirection.cpp\
        laplace.cpp\
				canny.cpp \
		pointandline.cpp \
		gradientmethod.cpp

HEADERS  += mainwindow.h\
            defineddirection.h\
            laplace.h\
						canny.h \
		pointandline.h \
		gradientmethod.h

FORMS    += \
    mainwindow.ui\
    defineddirection.ui\
    laplace.ui\
		canny.ui \
		pointandline.ui \
		gradientmethod.ui
