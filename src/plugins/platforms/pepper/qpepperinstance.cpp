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

#include "qpepperinstance.h"

#ifndef QT_PEPPER_STANDALONE_MODE
#include "qpepperhelpers.h"
#include "qpeppermodule.h"
#include "qpepperintegration.h"
#endif

#include <qpa/qwindowsysteminterface.h>

#include "ppapi/cpp/var.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/completion_callback.h"

using namespace pp;

QObject *qtScriptableObject;

QPepperInstance::QPepperInstance(PP_Instance instance)
: pp::Instance(instance)
{
    m_currentGeometry = Rect();
    m_context2D = 0;
    m_imageData2D = 0;
    m_frameBuffer = 0;
    m_inFlush = false;
    m_callbackFactory.Initialize(this);
}

QPepperInstance::~QPepperInstance()
{
    delete m_context2D;
    delete m_imageData2D;
    delete m_frameBuffer;
}

bool QPepperInstance::Init(uint32_t argc, const char* argn[], const char* argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argn);
    Q_UNUSED(argv);

    m_windowId = 0;
    for (unsigned int i = 0; i < argc; ++i) {
        //qDebug() << "arg" << argn[i] << argv[i];
        if (qstrcmp(argn[i], "qt_window_id") == 0) {
            m_windowId = QByteArray(argv[i]).toInt();
        }
    }

//    qDebug() << "PepperInstance::init" << m_windowId;

    RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL | PP_INPUTEVENT_CLASS_KEYBOARD);

    m_pepperIntegraton = QPepperIntegration::getPepperIntegration();
    m_pepperIntegraton->setPepperInstance(this);
    return true;
}

void QPepperInstance::DidChangeView(const Rect& geometry, const Rect& clip)
{
    Q_UNUSED(clip);
    if (geometry.size() == m_currentGeometry.size())
        return;
    m_currentGeometry = geometry;

 //   qDebug() << "QPepperInstance::DidChangeView" << m_windowId
 //            << geometry.size().width() << geometry.size().height();

    if (m_pepperIntegraton->wantsOpenGLGraphics()) {


    } else {
        // Delete the previous framebuffer and graphics context.
        delete m_context2D;
        delete m_imageData2D;
        delete m_frameBuffer;

        // Create new graphics context and frame buffer.
        m_context2D = new Graphics2D(this, geometry.size(), false);
        if (!BindGraphics(*m_context2D)) {
            printf("Couldn't bind the device context\n");
        }

        m_imageData2D = new ImageData(this, PP_IMAGEDATAFORMAT_BGRA_PREMUL, geometry.size(), true);
        m_frameBuffer = new QImage(reinterpret_cast<uchar *>(m_imageData2D->data()),
                               geometry.size().width(), geometry.size().height(),
                               m_imageData2D->stride(), QImage::Format_ARGB32_Premultiplied);


        m_pepperIntegraton->setRasterFrameBuffer(m_frameBuffer);
    }
}

void QPepperInstance::DidChangeFocus(bool has_focus)
{
    Q_UNUSED(has_focus);
}

bool QPepperInstance::HandleInputEvent(const pp::InputEvent& event)
{
    bool ret = m_pepperIntegraton->pepperEventTranslator()->processEvent(event);
    m_pepperIntegraton->processEvents();
    return ret;
}

bool QPepperInstance::HandleDocumentLoad(const URLLoader& url_loader)
{
    Q_UNUSED(url_loader);
    return false;
}

// HandleMessage expects a message of the format "funcitonName:argument", or
// "functionName" and will call the corresponding slot on qtScriptableObject.
void QPepperInstance::HandleMessage(const Var& var_message)
{
    if (qtScriptableObject == 0)
        return;

    QString message = QString::fromUtf8(var_message.AsString().data());

    QStringList parts = message.split(QLatin1Char(':'));
    if (parts.count() == 0)
        return; // Empty or ill-formed message

    const char *methodName = parts.at(0).toLatin1().constData();

    // Call the function on the Qt main thread using Qt::QueuedConnection since
    // this HandleMessage is called from the pepper tread. Currently zero or one
    // arguments are supported.
    if (parts.count() == 1) {
        QMetaObject::invokeMethod(qtScriptableObject, methodName, Qt::QueuedConnection);
    } else if (parts.count() == 2) {
        QString argument = parts.at(1);
        QMetaObject::invokeMethod(qtScriptableObject, methodName, Qt::QueuedConnection, Q_ARG(QString, argument));
    } else {
        qWarning("QtInstance::HandleMessage supports messages with zero or one arguements only");
    }
}

void QPepperInstance::flush()
{
    if (!m_context2D) {
        m_inFlush = false;
        return;
    }

    m_context2D->PaintImageData(*m_imageData2D, pp::Point(),
                                 pp::Rect(0, 0, m_context2D->size().width(),
                                                m_context2D->size().height()));
    m_context2D->Flush(m_callbackFactory.NewCallback(&QPepperInstance::flushCompletedCallback));

    m_inFlush = true;
}

void QPepperInstance::waitForFlushed()
{
    if (!m_inFlush)
        return;
}

void QPepperInstance::flushCompletedCallback(int32_t)
{
    m_inFlush = false;
    m_pepperIntegraton->flushCompleted();

    //qDebug() << "flushCompleted" << QThread::currentThreadId();
}

void flush_callback(void *, int32_t)
{

}

void QPepperInstance::setupTestGraphics(pp::Size newSize)
{
    QSize qtSize = toQSize(newSize);

    m_context2D = new Graphics2D(this, newSize, false);
    if (!BindGraphics(*m_context2D)) {
        printf("Couldn't bind the device context\n");
    }

    m_imageData2D = new ImageData(this, PP_IMAGEDATAFORMAT_BGRA_PREMUL, newSize, true);

    QImage qtImage = QImage(reinterpret_cast<uchar *>(m_imageData2D->data()),
                     qtSize.width(), qtSize.height(),
                     m_imageData2D->stride(),
                     QImage::Format_ARGB32_Premultiplied);

    qtImage.fill(Qt::red);
    m_context2D->PaintImageData(*m_imageData2D, pp::Point(0,0));
    //m_context2D->Flush(pp::CompletionCallback(flush_callback, 0));
}
