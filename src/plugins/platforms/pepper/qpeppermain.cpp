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

#include "qpeppermain.h"
#include "peppermodule.h"
#include <ppapi/cpp/completion_callback.h>
#include <qdebug.h>
#include <qthread.h>
#include <qfile.h>
#include <fstream>
#include <iostream>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppp.h"

Q_GLOBAL_STATIC(QtPepperMain, qtPepperMain);

QtPepperMain *QtPepperMain::get()
{
    return qtPepperMain();
}

QtPepperMain::QtPepperMain()
:m_mainInstance(0)
,m_exitNow(false) // set when Qt should exit immediately.
{

}

extern "C" { int PpapiPluginMain(void); }

void QtPepperMain::execPepper()
{
    qDebug() << "QtPepperMain::execPepper";
    PpapiPluginMain();
}

void QtPepperMain::postJavascriptMessage(const QByteArray &message)
{
   /* QPepperInstance *instance = QtPepperMain::get()->m_mainInstance;
    if (instance) {
        instance->PostMessage(pp::Var(message.constData()));
    }
    */
}

void QtPepperMain::setInstance(QPepperInstance *instance)
{
    m_mainInstance = instance;
//    extern pp::Instance *qtPepperInstance; // qglobal.cpp
//    qtPepperInstance = m_mainInstance;
    m_compositor.setPepperInstance(instance);
}

void qtPepperMessageHandlerPrinter(void *msg)
{
    const char * test = reinterpret_cast<const char *>(msg);
    printf("%s\n", test);
    free(msg);
}

void qtPepperMessageHandler(QtMsgType typ, const char *msg)
{
    qtPepperMessageHandlerPrinter(qstrdup(msg));
}

