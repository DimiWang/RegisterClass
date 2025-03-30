QT       += core gui
RC_ICONS = regdecode.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets  axcontainer
DEFINES += QT_NO_WHATSTHIS  QT_USE_QSTRINGBUILDER
#QMAKE_LFLAGS += -static
CONFIG += c++11

Release:DESTDIR =   $${PWD}/../bin
Release:OBJECTS_DIR = $${PWD}/../bin/release/.obj
Release:MOC_DIR =   $${PWD}/../bin/release/.moc
Release:RCC_DIR =   $${PWD}/../bin/release/.rcc
Release:UI_DIR =    $${PWD}/../bin/release/.ui

Debug:DESTDIR =     $${PWD}/../bin/debug
Debug:OBJECTS_DIR = $${PWD}/../bin/debug/.obj
Debug:MOC_DIR =     $${PWD}/../bin/debug/.moc
Debug:RCC_DIR =     $${PWD}/../bin/debug/.rcc
Debug:UI_DIR =      $${PWD}/../bin/debug/.ui


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ccodegen.cpp \
    dlgfield.cpp \
    lineedit.cpp \
    main.cpp \
    mainwindow.cpp\     \
    memorymap.cpp

HEADERS += \
    ccodegen.h \
    dlgfield.h \
    lineedit.h \     \
    mainwindow.h \
    memorymap.h
    mainwindow.h

FORMS += \
    dlgfield.ui \
    mainwindow.ui

# Default rules for deployment.


include(../register/register.pri)


RESOURCES += \
    regdecode.qrc
