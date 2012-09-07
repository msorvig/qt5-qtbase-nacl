/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpepperintegration.h"
#include "qpepperscreen.h"
#include "qpepperplatformwindow.h"
#include "qpepperfontdatabase.h"
#include "qpepperbackingstore.h"
#include "qpeppereventdispatcher.h"
#include "qpeppertheme.h"

#include <qpa/qplatformwindow.h>
#include <qpa/qwindowsysteminterface.h>

#include <QtGui/QSurface>
#include <qdebug.h>

QPepperIntegration * QPepperIntegration::createPepperIntegration()
{
    return new QPepperIntegration();
}

static QPepperIntegration *globalPepperIntegration;
QPepperIntegration *QPepperIntegration::getPepperIntegration()
{
    return globalPepperIntegration;
}

QPepperIntegration::QPepperIntegration()
    : m_firstWindowCreated(false)
{
    globalPepperIntegration = this;

    m_screen = new QPepperScreen();
    screenAdded(m_screen);

    m_compositor = new QPepperCompositor();
    m_eventTranslator = new PepperEventTranslator();
    QObject::connect(m_eventTranslator, SIGNAL(getWindowAt(QPoint,QWindow**)), this, SLOT(getWindowAt(QPoint,QWindow**)));
    QObject::connect(m_eventTranslator, SIGNAL(getKeyWindow(QWindow**)), this, SLOT(getKeyWindow(QWindow**)));

//    qDebug() << "QPepperIntegration::QPepperIntegration()";

    m_fontDatabase = 0;
    m_pepperEventDispatcher = new QPepperEventDispatcher();
//    qDebug() << "QPepperIntegration::QPepperIntegration done()";
}

QPepperIntegration::~QPepperIntegration()
{
    globalPepperIntegration = 0;
    delete m_compositor;
    delete m_eventTranslator;
    delete m_fontDatabase;
    delete m_pepperEventDispatcher;
}

bool QPepperIntegration::hasOpenGL() const
{
    return true;
}

QPlatformOpenGLContext *QPepperIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    return new QPepperGLContext();
}

QPlatformWindow *QPepperIntegration::createPlatformWindow(QWindow *window) const
{
    //qDebug() << "QPepperIntegration::createPlatformWindow" << window << window->parent()
    //         << window->objectName() << window->geometry() << m_firstWindowCreated;

    QPepperPlatformWindow *platformWindow = new QPepperPlatformWindow(window);
    useOpenglToplevel = (window->surfaceType() == QSurface::OpenGLSurface);
/*
    platformWindow->m_trackInstanceSize = true;
    if (!m_firstWindowCreated) {
        window->setWindowState(Qt::WindowFullScreen);
        window->resize(QtPepperMain::get()->m_screen->geometry().size());
    }
    m_firstWindowCreated = true;
*/
    return platformWindow;
}

QPlatformBackingStore *QPepperIntegration::createPlatformBackingStore(QWindow *window) const
{
    // qDebug() << "createPlatformBackingStore::createWindowBackingStore" << window << window->parent();
    QPepperBackingStore *backingStore = new QPepperBackingStore(window);
    return backingStore;
}

QAbstractEventDispatcher* QPepperIntegration::guiThreadEventDispatcher() const
{
//    qDebug() << "QPepperIntegration::guiThreadEventDispatcher()";
    return m_pepperEventDispatcher;
}

QPlatformFontDatabase *QPepperIntegration::fontDatabase() const
{
    if (m_fontDatabase == 0)
        m_fontDatabase = new QPepperFontDatabase();

    return m_fontDatabase;
}

QStringList QPepperIntegration::themeNames() const
{
    return QStringList() << QStringLiteral("pepper");
}

QPlatformTheme *QPepperIntegration::createPlatformTheme(const QString &name) const
{
    if (name == QStringLiteral("pepper"))
        return new QPepperTheme;

    return 0;
}

void QPepperIntegration::setPepperInstance(QPepperInstance *instance)
{
    m_pepperInstance = instance;
    connect(m_compositor,SIGNAL(flush()), this, SLOT(flushRasterFrameBuffer()));
}

QPepperInstance *QPepperIntegration::pepperInstance() const
{
    return m_pepperInstance;
}

QPepperCompositor *QPepperIntegration::pepperCompositor() const
{
    return m_compositor;
}

PepperEventTranslator *QPepperIntegration::pepperEventTranslator()
{
    return m_eventTranslator;
}

void QPepperIntegration::processEvents()
{
    m_pepperEventDispatcher->processEventsContinue();
}

bool QPepperIntegration::wantsOpenGLGraphics() const
{
    return useOpenglToplevel;
}

void QPepperIntegration::setRasterFrameBuffer(QImage *m_frameBuffer)
{
    // Set the frame buffer on the compositor
    m_compositor->setRasterFrameBuffer(m_frameBuffer);
    m_compositor->composit();

    QWindowSystemInterface::handleScreenGeometryChange(m_screen->screen(), toQRect(m_pepperInstance->m_currentGeometry));

    // Let Qt process events;
    m_pepperEventDispatcher->processEventsContinue();
}

void QPepperIntegration::getWindowAt(const QPoint & point, QWindow **window)
{
    *window = m_compositor->windowAt(point);
}

void QPepperIntegration::getKeyWindow(QWindow **window)
{
    *window = m_compositor->keyWindow();
}

void QPepperIntegration::flushRasterFrameBuffer()
{
    m_pepperInstance->flush();
}


