TARGET = qtminimalpepper
load(qt_plugin)

QT += core-private gui-private platformsupport-private
DESTDIR = $$QT.gui.plugins/platforms

SOURCES =   main.cpp \
            qminimalpepperintegration.cpp \
            qminimalpepperbackingstore.cpp
HEADERS =   qminimalpepperintegration.h \
            qminimalpepperbackingstore.h

OTHER_FILES += minimalpepper.json

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
