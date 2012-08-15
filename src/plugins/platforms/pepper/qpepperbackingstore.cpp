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

#include "qpepperbackingstore.h"
#include "qpeppercompositor.h"
#include "qpeppermain.h"

#include <QtCore/qdebug.h>
#include <QtGui/QPainter>

QT_BEGIN_NAMESPACE


QPepperBackingStore::QPepperBackingStore(QWindow *window)
    : QPlatformBackingStore(window)
{
    m_isInPaint = false;
    m_frameBuffer = 0;
    m_compositor = &QtPepperMain::get()->m_compositor;
    resize(window->size(), QRegion());
}

QPepperBackingStore::~QPepperBackingStore()
{
    if (m_ownsFrameBuffer)
        delete m_frameBuffer;
}

QPaintDevice *QPepperBackingStore::paintDevice()
{
    // qDebug() << "QPepperBackingStore::paintDevice framebuffer" << m_frameBuffer;
    return m_frameBuffer;
}

void QPepperBackingStore::beginPaint(const QRegion &region)
{
    // qDebug() << "QPepperBackingStore::beginPaint" << window();
    m_isInPaint = true;

    m_compositor->waitForFlushed(window()->handle());
    // qDebug() << "QPepperBackingStore::beginPaint done";
}

void QPepperBackingStore::endPaint()
{
    // qDebug() << "QPepperBackingStore::endPaint";
    m_isInPaint = false;
}


void QPepperBackingStore::flush(QWindow *window, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(window); // ### TODO: complications if widget
                      // is a child widget. (see QWindowSurface::flush)
    Q_UNUSED(region);
    Q_UNUSED(offset);

    // qDebug() << "QPepperBackingStore::flush" << this->window();

    m_compositor->flush(window->handle());
}

void QPepperBackingStore::resize(const QSize &size, const QRegion &)
{
  //  qDebug() << "QPepperBackingStore::resize" << size;
  //  if (m_frameBuffer)
  //      qDebug() << "resize" << m_frameBuffer->size();


    if (!m_frameBuffer || size != m_frameBuffer->size()) {
        createFrameBuffer(size);
        m_size = size;
    }
}

void QPepperBackingStore::createFrameBuffer(QSize size)
{
    //if (size.isValid())
    //    return;
    // qDebug() << "QPepperBackingStore::createFrameBuffer" << size;

    if (m_ownsFrameBuffer)
        delete m_frameBuffer;
    m_frameBuffer = new QImage(size, QImage::Format_ARGB32_Premultiplied);
    m_frameBuffer->fill(Qt::red); // should never be seen
    m_ownsFrameBuffer = true;

    m_compositor->setFrameBuffer(window()->handle(),  m_frameBuffer);

}

void QPepperBackingStore::setFrameBuffer(QImage *frameBuffer)
{
//    if (m_isInPaint)
        qFatal("QPepperBackingStore::setFrameBuffer called.");

    if (m_ownsFrameBuffer)
        delete m_frameBuffer;
    m_frameBuffer = frameBuffer;
    m_ownsFrameBuffer = false;

    window()->resize(frameBuffer->size());
}

QT_END_NAMESPACE
