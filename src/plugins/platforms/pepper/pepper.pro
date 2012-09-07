# DEFINES += QT_PEPPER_USE_PEPPER_FONT_ENGINE
TARGET = qtpepper
load(qt_plugin)
DESTDIR = $$QT.gui.plugins/platforms

#include($$PWD/../../../qbase.pri)
#include($$PWD/../../qpluginbase.pri)
#include($$PWD/../fontdatabases/genericunix/genericunix.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/pepperpluginmain.cpp \
           $$PWD/pepperinstance.cpp \
           $$PWD/peppermodule.cpp \
           $$PWD/qpepperfontdatabase.cpp \
           $$PWD/qpepperscreen.cpp \
           $$PWD/peppereventtranslator.cpp \
           $$PWD/qpepperintegration.cpp \
           $$PWD/qpepperplatformwindow.cpp \
           $$PWD/qpepperbackingstore.cpp \
           $$PWD/qpepperfontengine.cpp \
           $$PWD/qpepperglcontext.cpp \
           $$PWD/qpepperhelpers.cpp \
           $$PWD/qpeppercompositor.cpp \
           $$PWD/qpeppereventdispatcher.cpp \

HEADERS += $$PWD/qpepperhelpers.h \
           $$PWD/peppereventtranslator.h \
           $$PWD/qpepperintegration.h \
           $$PWD/qpepperplatformwindow.h \
           $$PWD/qpepperbackingstore.h \
           $$PWD/pepperinstance.h \
           $$PWD/peppermodule.h \
           $$PWD/qpepperfontdatabase.h \
           $$PWD/qpepperscreen.h \
           $$PWD/qpepperfontengine.h \
           $$PWD/qpepperglcontext.h \
           $$PWD/qpepperhelpers.h \
           $$PWD/qpeppercompositor.h \
           $$PWD/qpeppereventdispatcher.h \


RESOURCES += $$PWD/../../../../lib/fonts/naclfonts.qrc

OTHER_FILES += $$PWD/../../../../tools/nacldemoserver/check_browser.js \
               $$PWD/../../../../tools/nacldemoserver/qtnaclloader.js

QT += core-private gui-private platformsupport-private

LIBS += -lppruntime -lppapi -lppapi_gles2 -lsrpc -limc_syscalls -lplatform -lgio -lppapi_cpp
OBJECTS_DIR = .obj
MOC_DIR = .moc

OTHER_FILES += pepper.json
target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target

