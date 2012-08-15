# This file is included via the "nacl" feature,
# which is set by the mkspec.

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

