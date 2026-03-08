#include "controller.h"
#include "const.h"
#include "config.h"


namespace Steinberg {
namespace Vst {


FUnknown* VelocityController::createInstance(void*){
    return (IEditController*)new VelocityController();
}


tresult PLUGIN_API VelocityController::initialize(FUnknown* context){
    tresult result = EditController::initialize(context);
    if(result == kResultTrue){
        // add initialize here if needed
    }
    return result;
}


} }