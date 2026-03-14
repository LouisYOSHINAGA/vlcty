#include "controller.h"
#include "config.h"
#include "param.h"


namespace Steinberg {
namespace Vst {


FUnknown* VelocityController::createInstance(void*){
    return (IEditController*)new VelocityController();
}


tresult PLUGIN_API VelocityController::initialize(FUnknown* context){
    tresult result = EditController::initialize(context);
    if(result != kResultTrue){
        return result;
    }

    DiscreteRangeParameter* velocity = new DiscreteRangeParameter(
        STR16("Velocity"),  // title
        PARAM_ID_VELOCITY,  // tag
        nullptr,  // units
        127,  // step count
        0,  // min plain
        127,  // max plain
        64  // default value plain
    );
    parameters.addParameter(velocity);

    return result;
}

IPlugView* PLUGIN_API VelocityController::createView(const char* name){
    if(strcmp(name, "editor") == 0){
        VelocityGUIEditor* view = new VelocityGUIEditor(this);
        return view;
    }
    return 0;
}


} }