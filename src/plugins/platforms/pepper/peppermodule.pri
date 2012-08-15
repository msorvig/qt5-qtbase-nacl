# peppermodule.cpp contains the main entry point for native client
# applications: Module* CreateModule().

# When linking statically this file has to be compiled
# into the application itself - if it's included in an archive
# CreateModule() wont be references from anywhere and left out
# of the final executable.


INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/peppermodule.cpp
HEADERS += $$PWD/peppermodule.h

RESOURCES += $$PWD/../../../../lib/fonts/naclfonts.qrc
QT += nacl
CONFIG += static
LIBS += -lppapi -lppapi_cpp -lppapi_gles2 -lsrpc -limc_syscalls -lplatform -lgio
OBJECTS_DIR = .obj/debug-static
MOC_DIR = .moc/debug-static
TEMPLATE = app

