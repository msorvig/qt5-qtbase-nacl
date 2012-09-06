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

#include "qpeppereventdispatcher.h"
#include "qpeppermain.h"
#include "peppermodule.h"
#include <qdebug.h>

QPepperEventDispatcher::QPepperEventDispatcher(QObject *parent)
:QUnixEventDispatcherQPA(parent)
,firstProcessEventsCall(true)
,pepperInitialized(false)
,completionCallbackFactory(0) // ppapi not started yet, to early to create.
{
    qDebug() << "QPepperEventDispatcher";
}

QPepperEventDispatcher::~QPepperEventDispatcher()
{
    delete completionCallbackFactory;
}

void QPepperEventDispatcher::registerTimer(int timerId, int interval, Qt::TimerType timerType, QObject *object)
{
   // qDebug() << "           QPepperEventDispatcher::registerTimer" << timerId << interval;

    PepperTimerInfo timerInfo(timerId, interval, timerType, object);
    if (pepperInitialized)
        startTimer(timerInfo);
    else
        preTimers.insert(timerId, timerInfo);

 //   QUnixEventDispatcherQPA::registerTimer(timerId, interval, timerType, object);
}

bool QPepperEventDispatcher::unregisterTimer(int timerId)
{
  //  qDebug() << "             QPepperEventDispatcher::unregisterTimer" << timerId;
    preTimers.remove(timerId);
    timers.remove(timerId);
    return true;
//    return QUnixEventDispatcherQPA::unregisterTimer(timerId);
}

bool QPepperEventDispatcher::unregisterTimers(QObject *object)
{
  //  qDebug() << "             QPepperEventDispatcher::unregisterTimers" << object;
    // return QUnixEventDispatcherQPA::unregisterTimers(object);
    return false;
}

bool QPepperEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    if (firstProcessEventsCall) {
        firstProcessEventsCall = false;
        qDebug() << "QPepperEventDispatcher::processEvents first";

        // At this point app.exec() has been called and control
        // has been passed to Qt. Call startPepper which will
        // call
        QtPepperMain::get()->m_eventDispatcher = this;
        QtPepperMain::execPepper();

        // At this point the application is shutting down.

        qDebug() << "QPepperEventDispatcher::processEvents first done";

    }
    return QUnixEventDispatcherQPA::processEvents(flags);
}

bool QPepperEventDispatcher::processEventsContinue()
{
    //    qDebug() << "QPepperEventDispatcher::processEventsContinue";

    if (!pepperInitialized) {
        qDebug() << "QPepperEventDispatcher::processEventsContinue first";
        pepperInitialized = true;

        QHash<int, PepperTimerInfo>::iterator it = preTimers.begin();
        QHash<int, PepperTimerInfo>::iterator end = preTimers.end();
        while(it != end) {
            startTimer(it.value());
            ++it;
        }
        preTimers.clear();
    }


    return QUnixEventDispatcherQPA::processEvents(QEventLoop::EventLoopExec);
}

bool QPepperEventDispatcher::hasPendingEvents()
{
    qDebug() << "QPepperEventDispatcher::hasPendingEvents";
    return QUnixEventDispatcherQPA::hasPendingEvents();
}

void QPepperEventDispatcher::flush()
{
    qDebug() << "QPepperEventDispatcher::flush";
    QUnixEventDispatcherQPA::flush();
}

void QPepperEventDispatcher::startTimer(PepperTimerInfo info)
{
    if (!completionCallbackFactory) {
        completionCallbackFactory = new pp::CompletionCallbackFactory<QPepperEventDispatcher>(this);
    }

   // qDebug() << "           start timer" << info.timerId << info.interval;
    if (!timers.contains(info.timerId))
        timers.insert(info.timerId, info);

    QtModule::getCore()->CallOnMainThread(info.interval,
        completionCallbackFactory->NewCallback(&QPepperEventDispatcher::timerCallback), info.timerId);
}

void QPepperEventDispatcher::timerCallback(int32_t timerId)
{
 //   QUnixEventDispatcherQPA::activateTimers();

    if (!timers.contains(timerId)) {
        return;
    }

    PepperTimerInfo info = timers.value(timerId);
    if (timers.contains(timerId))
        startTimer(timers.value(timerId));

    // qDebug() << "QPepperEventDispatcher::timer activate" << timerId << info.object;

    QTimerEvent e(info.timerId);
    QCoreApplication::sendEvent(info.object, &e);

    processEventsContinue();

}
