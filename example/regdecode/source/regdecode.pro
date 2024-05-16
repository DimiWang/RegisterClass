QT       += core gui
RC_ICONS = regdecode.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_NO_WHATSTHIS  QT_USE_QSTRINGBUILDER Q_FOREACH
#QMAKE_LFLAGS += -static
CONFIG += c++11

Release:DESTDIR = $${PWD}/../
Release:OBJECTS_DIR = $${PWD}/release/.obj
Release:MOC_DIR = $${PWD}/release/.moc
Release:RCC_DIR = $${PWD}/release/.rcc
Release:UI_DIR = $${PWD}/release/.ui

Debug:DESTDIR = $${PWD}/debug
Debug:OBJECTS_DIR = $${PWD}/debug/.obj
Debug:MOC_DIR = $${PWD}/debug/.moc
Debug:RCC_DIR = $${PWD}/debug/.rcc
Debug:UI_DIR = $${PWD}/debug/.ui


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lineedit.cpp \
    main.cpp \
    mainwindow.cpp\    

HEADERS += \
    lineedit.h \    
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(../../2_bitfield/source/register.pri)


RESOURCES += \
    regdecode.qrc
