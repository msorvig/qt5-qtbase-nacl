# DEFINES += QT_PEPPER_USE_PEPPER_FONT_ENGINE
DEFINES += QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION
load(qt_module)

TARGET     = QtNacl
TEMPLATE = lib
MODULE = nacl
MODULE_PRI = ../modules/qt_nacl.pri
#DESTDIR    = $$QMAKE_LIBDIR_QT

CONFIG += module

load(qt_module_config)


#include($$PWD/../../../qbase.pri)
#include($$PWD/../../qpluginbase.pri)

#include($$PWD/../fontdatabases/genericunix/genericunix.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/lighthousepluginmain.cpp \
           $$PWD/pepperinstance.cpp \
           $$PWD/qpepperfontdatabase.cpp \
           $$PWD/qpeppermain.cpp \
           $$PWD/qpepperscreen.cpp \
           $$PWD/peppereventtranslator.cpp \
           $$PWD/qpepperintegration.cpp \
           $$PWD/qpepperplatformwindow.cpp \
           $$PWD/qpepperbackingstore.cpp \
           $$PWD/qpepperfontengine.cpp \
#           $$PWD/qpepperglcontext.cpp \
           $$PWD/qpepperhelpers.cpp \
           $$PWD/qpeppercompositor.cpp \

HEADERS += $$PWD/peppereventtranslator.h \
           $$PWD/qpepperintegration.h \
           $$PWD/qpepperplatformwindow.h \
           $$PWD/qpepperbackingstore.h \
           $$PWD/pepperinstance.h \
           $$PWD/qpepperfontdatabase.h \
           $$PWD/qpeppermain.h \
           $$PWD/qpepperscreen.h \
           $$PWD/qpepperfontengine.h \
#           $$PWD/qpepperglcontext.h \
           $$PWD/qpepperhelpers.h \
           $$PWD/qpeppercompositor.h \

RESOURCES += $$PWD/../../../../lib/fonts/naclfonts.qrc

OTHER_FILES += $$PWD/../../../../tools/nacldemoserver/check_browser.js \
               $$PWD/../../../../tools/nacldemoserver/qtnaclloader.js

QT += core-private gui-private platformsupport-private

CONFIG += static
LIBS += -lppapi -lppapi_cpp -lppapi_gles2 -lsrpc -limc_syscalls -lplatform -lgio
OBJECTS_DIR = .obj
MOC_DIR = .moc


#target.path += $$[QT_INSTALL_LIBS]
#INSTALLS += target


