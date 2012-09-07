/*
    Licensed under the MIT license.
*/
#include "peppermodule.h"
#include "pepperinstance.h"

#include <GLES2/gl2.h>
#include <ppapi/gles2/gl2ext_ppapi.h>
#include <ppapi/c/ppp.h>

pp::Core *globalCore;

QtModule::QtModule()
{

}

QtModule::~QtModule()
{

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

PP_EXPORT Module* CreateModule() {
    return new QtModule();
}

}
