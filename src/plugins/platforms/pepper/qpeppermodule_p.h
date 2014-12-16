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

#ifndef QT_PEPPER_MODULE_P_H
#define QT_PEPPER_MODULE_P_H

#include "ppapi/cpp/core.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/instance.h"

class QPepperModule;
class QPepperModulePrivate {
public:
    QPepperModulePrivate(QPepperModule *module);
    bool init();
    pp::Instance* createInstance(PP_Instance ppInstance);
    static pp::Core* core();
private:
    QPepperModule *q;
};

#endif
