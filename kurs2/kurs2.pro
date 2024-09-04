QT       += core gui
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    admin.cpp \
    client.cpp \
    clientkredit.cpp \
    clientvklad.cpp \
    kredit.cpp \
    main.cpp \
    mainwindow.cpp \
    schet.cpp \
    user.cpp \
    userclient.cpp \
    userkredit.cpp \
    uservklad.cpp \
    vklad.cpp

HEADERS += \
    admin.h \
    client.h \
    clientkredit.h \
    clientvklad.h \
    kredit.h \
    mainwindow.h \
    schet.h \
    user.h \
    userclient.h \
    userkredit.h \
    uservklad.h \
    vklad.h

FORMS += \
    admin.ui \
    client.ui \
    clientkredit.ui \
    clientvklad.ui \
    kredit.ui \
    mainwindow.ui \
    schet.ui \
    user.ui \
    userclient.ui \
    userkredit.ui \
    uservklad.ui \
    vklad.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
