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

#include "qpepperplatformwindow.h"
#include "qpepperglcontext.h"
#include "qpeppermodule.h"
#include "qpeppercompositor.h"
#include <QtGui/QSurface>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QPepperPlatformWindow::QPepperPlatformWindow(QWindow *window)
:QPlatformWindow(window)
,m_isVisible(false)
{
    m_pepperIntegraion = QPepperIntegration::getPepperIntegration();
    m_compositor = m_pepperIntegraion->pepperCompositor();
    m_useCompositor = (window->surfaceType() == QSurface::RasterSurface);

    if (m_useCompositor)
        m_compositor->addRasterWindow(this);

//    qDebug() << "QPepperPlatformWindow::QPepperPlatformWindow";
}

QPepperPlatformWindow::~QPepperPlatformWindow()
{
    if (m_useCompositor)
        m_compositor->removeWindow(this);
}

WId QPepperPlatformWindow::winId() const
{
    return WId(this);
}

void QPepperPlatformWindow::setVisible(bool visible)
{
//    qDebug() << "QPepperPlatformWindow::setVisible" << visible;
    QWindowSystemInterface::handleSynchronousExposeEvent(window(), QRegion(window()->geometry()));
    if (visible)
        QWindowSystemInterface::handleSynchronousExposeEvent(window(), QRegion(window()->geometry()));

    if (m_useCompositor)
        m_compositor->setVisible(this, visible);
//    qDebug() << "QPepperPlatformWindow::setVisible done";
}

void QPepperPlatformWindow::raise()
{
    if (m_useCompositor)
        m_compositor->raise(this);
}

void QPepperPlatformWindow::lower()
{
    if (m_useCompositor)
        m_compositor->lower(this);
}

void QPepperPlatformWindow::setGeometry(const QRect &rect)
{
//    qDebug() << "QPepperPlatformWindow::setGeometry" << rect;
    QPlatformWindow::setGeometry(rect);
}

void QPepperPlatformWindow::setParent(const QPlatformWindow *parent)
{
    qDebug() << "QPepperPlatformWindow::setParent" << parent;
    if (m_useCompositor)
        m_compositor->setParent(this, const_cast<QPlatformWindow *>(parent));
}

bool QPepperPlatformWindow::setKeyboardGrabEnabled(bool grab)
{
    Q_UNUSED(grab);
    return false;
}

bool QPepperPlatformWindow::setMouseGrabEnabled(bool grab)
{
    Q_UNUSED(grab);
    return false;
}


QT_END_NAMESPACE
