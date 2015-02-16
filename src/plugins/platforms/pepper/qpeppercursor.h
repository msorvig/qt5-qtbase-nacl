/****************************************************************************
**
** Copyright (C) 2014 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com <http://qt.digia.com/>
**
** This file is part of the Qt Native Client platform plugin.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** If you have questions regarding the use of this file, please use
** contact form at http://qt.digia.com <http://qt.digia.com/>
**
****************************************************************************/

#ifndef QPEPPERCURSOR_H
#define QPEPPERCURSOR_H

#include <QtCore/qloggingcategory.h>
#include <qpa/qplatformcursor.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(QT_PLATFORM_PEPPER_CURSOR);

class QPepperCursor : public QPlatformCursor
{
public:
    QPepperCursor();
    ~QPepperCursor();
    void changeCursor(QCursor *cursor, QWindow *window);

private:
};

QT_END_NAMESPACE

#endif
