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

#include "qpeppercompositor.h"

#include <QtGui>
#include <qpa/qplatformwindow.h>
#include <qpa/qwindowsysteminterface.h>

QPepperCompositedWindow::QPepperCompositedWindow()
    :window(0)
    ,frameBuffer(0)
    ,parentWindow(0)
    ,flushPending(false)
    ,visible(false)
{

}

QPepperCompositor::QPepperCompositor()
    :m_frameBuffer(0)
    ,m_needComposit(false)
    ,m_inFlush(false)
{

}

void QPepperCompositor::addRasterWindow(QPlatformWindow *window, QPlatformWindow *parentWindow)
{
//    qDebug() << "QPepperCompositor::addRasterWindow" << window << parentWindow << window->window() << window->window()->objectName();

    QPepperCompositedWindow compositedWindow;
    compositedWindow.window = window;
    compositedWindow.parentWindow = parentWindow;
    m_compositedWindows.insert(window->window(), compositedWindow);

    if (parentWindow == 0) {
        m_windowStack.append(window->window());
    } else {
        m_compositedWindows[parentWindow->window()].childWindows.append(window->window());
    }
}

void QPepperCompositor::removeWindow(QPlatformWindow *window)
{
    QPlatformWindow *platformWindow = m_compositedWindows[window->window()].parentWindow;
    if (platformWindow) {
        QWindow *parentWindow = platformWindow->window();
        m_compositedWindows[parentWindow].childWindows.removeAll(window->window());
    }
    m_windowStack.removeAll(window->window());
    m_compositedWindows.remove(window->window());
}

void QPepperCompositor::setVisible(QPlatformWindow *window, bool visible)
{
    QPepperCompositedWindow &compositedWindow = m_compositedWindows[window->window()];
    if (compositedWindow.visible == visible)
        return;

//    qDebug() << "QPepperCompositor::setVisible " << this << visible;

    compositedWindow.visible = visible;
    compositedWindow.flushPending = true;
    compositedWindow.damage = compositedWindow.window->geometry();
    maybeComposit();
}

void QPepperCompositor::raise(QPlatformWindow *window)
{
    m_windowStack.removeAll(window->window());
    m_windowStack.append(window->window());
}

void QPepperCompositor::lower(QPlatformWindow *window)
{
    m_windowStack.removeAll(window->window());
    m_windowStack.prepend(window->window());
}

void QPepperCompositor::setParent(QPlatformWindow *window, QPlatformWindow *parent)
{
    m_compositedWindows[window->window()].parentWindow = parent;
}

void QPepperCompositor::setFrameBuffer(QPlatformWindow *window, QImage *frameBuffer)
{
    m_compositedWindows[window->window()].frameBuffer = frameBuffer;
}

void QPepperCompositor::flush(QPlatformWindow *window)
{
//    qDebug() << "QPepperCompositor::flush" << window;

    QPepperCompositedWindow &compositedWindow = m_compositedWindows[window->window()];
    compositedWindow.flushPending = true;
    compositedWindow.damage = compositedWindow.window->geometry();
    maybeComposit();
}

void QPepperCompositor::waitForFlushed(QPlatformWindow *surface)
{
    if (!m_compositedWindows[surface->window()].flushPending)
        return;
}

void QPepperCompositor::setRasterFrameBuffer(QImage *frameBuffer)
{
    m_frameBuffer = frameBuffer;
    //m_frameBuffer->fill(Qt::green);
    m_frameBuffer->fill(Qt::transparent);

    foreach (QWindow *window, m_windowStack) {
        QWindowSystemInterface::handleExposeEvent(window, QRegion(window->geometry()));
    }
}

void QPepperCompositor::flushCompleted()
{
    m_inFlush = false;
    if (m_needComposit) {
        composit();
        m_needComposit = false;
    }
}

QWindow *QPepperCompositor::windowAt(QPoint p)
{
    int index = m_windowStack.count() -1;
    // qDebug() << "window at" << "point" << p << "window count" << index;

    while (index >= 0) {
        QPepperCompositedWindow &compositedWindow
                = m_compositedWindows[m_windowStack.at(index)];
        // qDebug() << "windwAt testing" << compositedWindow.window << compositedWindow.window->geometry();
        if (compositedWindow.visible && compositedWindow.window->geometry().contains(p))
            return m_windowStack.at(index);
        --index;
    }
    return 0;
}

QWindow *QPepperCompositor::keyWindow()
{
    return m_windowStack.at(m_windowStack.count() - 1);
}

void QPepperCompositor::maybeComposit()
{
    if (!m_inFlush)
        composit();
    else
        m_needComposit = true;
}

void QPepperCompositor::composit()
{
//    qDebug() << "QPepperCompositor::composit";

    if (!m_frameBuffer) {
        qWarning("QPepperCompositor: No frame buffer set");
        return;
    }

    QPainter p(m_frameBuffer);

    // ### for now, clear the entire frame.
    p.fillRect(QRect(QPoint(0,0), m_frameBuffer->size()), QBrush(Qt::transparent));

    QRegion damaged;
    foreach (QWindow *window, m_windowStack) {
        QPepperCompositedWindow &compositedWindow = m_compositedWindows[window];
     //   qDebug() << "unite" << compositedWindow.damage;
        damaged = damaged.united(compositedWindow.damage);
    //    qDebug() << "res" << damaged;
    }

    //foreach (const QRect &rect, damaged.rects()) {
    //    p.fillRect(rect, QBrush(Qt::transparent));
    //    qDebug() << "fill" << rect;
    //}

    QRegion painted;

    foreach (QWindow *window, m_windowStack) {
        QPepperCompositedWindow &compositedWindow = m_compositedWindows[window];

        if (compositedWindow.visible) {
            p.drawImage(compositedWindow.window->geometry().topLeft(), *compositedWindow.frameBuffer);
            painted += compositedWindow.damage;
        }

        compositedWindow.flushPending = false;
        compositedWindow.damage = QRect();
    }

    //QRegion needsClear = damaged - painted;
    //qDebug() << "needsclear" << needsClear;

    m_inFlush = true;
    emit flush();
}

