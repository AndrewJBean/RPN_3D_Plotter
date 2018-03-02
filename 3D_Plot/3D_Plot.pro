#-------------------------------------------------
#
# Project created by QtCreator 2014-09-30T12:52:52
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 'RPN 3D Plotting'
TEMPLATE = app

#QMAKE_MAC_SDK = macosx10.9
QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += main.cpp\
        mainwindow.cpp \
	glwidget.cpp \
	rpn.cpp \
    logicclass.cpp

HEADERS  += mainwindow.h \
	glwidget.h \
	rpn.h \
    logicclass.h

FORMS    += mainwindow.ui

mac: LIBS += -framework GLUT
else:unix|win32: LIBS += -lGLUT
