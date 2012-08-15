/*
    Licensed under the MIT license.
*/
#include "peppermodule.h"
#include "pepperinstance.h"

#include <GLES2/gl2.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

#ifndef QT_PEPPER_STANDALONE_MODE
#include "qpeppermain.h"
#endif

pp::Core *globalCore;

QtModule::QtModule()
{

}

QtModule::~QtModule()
{
    // Attempt sane shutdown:
    QtPepperMain *pepperMain = QtPepperMain::get();
    //QMutexLocker lock(&pepperMain->m_mutex);
    pepperMain->m_exitNow = true;
    QTimer::singleShot(0, qApp, SLOT(quit()));
    //    glTerminatePPAPI();
}

bool QtModule::Init()
{
    globalCore = core();
//    bool glOk = glInitializePPAPI(get_browser_interface()) == GL_TRUE;

    return true;
}

pp::Instance* QtModule::CreateInstance(PP_Instance instance)
{
    return new QPepperInstance(instance);
}

pp::Core *QtModule::getCore()
{
    return globalCore;
}

namespace pp {

Module* CreateModule() {
    return new QtModule();
}

}
