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

#include "qplatformwindowformat_qpa.h"
#include "qpepperglcontext.h"
#include "pepperinstance.h"

#include <ppapi/gles2/gl2ext_ppapi.h>
#include <ppapi/cpp/dev/context_3d_dev.h>
#include <ppapi/cpp/dev/surface_3d_dev.h>
#include <qdebug.h>

QPepperGLContext::QPepperGLContext(QPepperInstance *instance)
    :m_instance(instance)
{
    QPepperInstance *m_instance;
    m_instance->pp_instance();
    m_context = 0;
    m_surface = 0;
    m_pendingFlush = false;
}

QPepperGLContext::~QPepperGLContext()
{

}

void QPepperGLContext::makeCurrent()
{
    qDebug() << "QPepperGLContext::makeCurrent";
    return;

    if (!m_context || m_context->is_null()) {
        m_context = new pp::Context3D_Dev(*m_instance, 0, pp::Context3D_Dev(), NULL);
        if (m_context->is_null()) {
            glSetCurrentContextPPAPI(0);
            return;
        }

    m_surface = new pp::Surface3D_Dev(*m_instance, 0, NULL);
    m_context->BindSurfaces(*m_surface, *m_surface);
        m_instance->BindGraphics(*m_surface);
    }
    glSetCurrentContextPPAPI(m_surface->pp_resource());
}

void QPepperGLContext::doneCurrent()
{
    qDebug() << "QPepperGLContext::doneCurrent";
    return;
    m_instance->BindGraphics(pp::Surface3D_Dev());
    m_context->BindSurfaces(pp::Surface3D_Dev(), pp::Surface3D_Dev());
    glSetCurrentContextPPAPI(0);
}

namespace {
    void FlushCallback(void* data, int32_t result) {
        Q_UNUSED(result);
        static_cast<QPepperGLContext*>(data)->flushCallback();
    }
}

void QPepperGLContext::swapBuffers()
{
    qDebug() << "QPepperGLContext::swapBuffers";
    return;
    if (m_pendingFlush)
        return;

    m_pendingFlush = true;
   // m_surface->SwapBuffers(pp::CompletionCallback(&FlushCallback, this));
}

void QPepperGLContext::flushCallback()
{
    m_pendingFlush = false;
}

void* QPepperGLContext::getProcAddress(const QString& procName)
{
    qDebug() << "QPepperGLContext::getProcAddress" << procName;
   // const PPB_OpenGLES2_Dev* functionPointers = glGetInterfacePPAPI();

    if (procName == QLatin1String("glIsRenderbufferEXT")) {
     //   return reinterpret_cast<void *>(functionPointers->IsRenderbuffer);
    }
    // ### ... and so on for all functions.

    return 0;
}

QPlatformWindowFormat QPepperGLContext::platformWindowFormat() const
{
    qDebug() << "QPepperGLContext::platformWindowFormat";
    // ## which features?
    return QPlatformWindowFormat::DoubleBuffer | QPlatformWindowFormat::DepthBuffer;
}
