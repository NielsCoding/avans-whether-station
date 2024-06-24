QT += core gui sql charts network

HEADERS += \
	Client.h \
	HandleMessage.h \
	consts.h \
	dbconnector.h \
	main.h \
	mainwindow.h

SOURCES += \
	Client.cpp \
	HandleMessage.cpp \
	dbconnector.cpp \
	main.cpp \
	mainwindow.cpp


FORMS += \
	dbconnector.ui \
	mainwindow.ui

INSTALLS += target
CONFIG += force_debug_info
