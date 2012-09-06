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
#include "qpeppermain.h"
#include "qpepperfontdatabase.h"
#include "qpepperbackingstore.h"
#include "qpeppereventdispatcher.h"

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>
#include <qdebug.h>

QPepperIntegration * QPepperIntegration::createPepperIntegration()
{
    return new QPepperIntegration();
}

QPepperIntegration::QPepperIntegration()
    : m_firstWindowCreated(false)
{

    qDebug() << "QPepperIntegration::QPepperIntegration()";
    QPepperScreen *screen = new QPepperScreen();
    QtPepperMain::get()->m_screen = screen;
    screenAdded(screen);

    m_fontDatabase = 0;
    m_eventDispatcherUnix = new QPepperEventDispatcher;
    qDebug() << "QPepperIntegration::QPepperIntegration done()";
}

QPepperIntegration::~QPepperIntegration()
{
    delete m_fontDatabase;
    delete m_eventDispatcherUnix;
}

bool QPepperIntegration::hasOpenGL() const
{
    qDebug() << "PepperIntegration::hasOpenGL";
    return false;
}


QPlatformWindow *QPepperIntegration::createPlatformWindow(QWindow *window) const
{
    //qDebug() << "QPepperIntegration::createPlatformWindow" << window << window->parent()
    //         << window->objectName() << window->geometry() << m_firstWindowCreated;

    QPepperPlatformWindow *platformWindow = new QPepperPlatformWindow(window, !m_firstWindowCreated);
    platformWindow->m_trackInstanceSize = true;
    if (!m_firstWindowCreated) {
        window->setWindowState(Qt::WindowFullScreen);
        window->resize(QtPepperMain::get()->m_screen->geometry().size());
    }
    m_firstWindowCreated = true;
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
    qDebug() << "QPepperIntegration::guiThreadEventDispatcher()";
    return m_eventDispatcherUnix;
}

QPlatformFontDatabase *QPepperIntegration::fontDatabase() const
{
    if (m_fontDatabase == 0)
        m_fontDatabase = new QPepperFontDatabase();

    return m_fontDatabase;
}
