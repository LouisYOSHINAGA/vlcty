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

    DiscreteRangeParameter* velocityFix = new DiscreteRangeParameter(
        STR16("Velocity Fix"),  // title
        PARAM_ID_VELOCITY_FIX,  // tag
        nullptr,  // units
        127,  // step count
        0,  // min plain
        127,  // max plain
        DEFAULT_VELOCITY  // default value plain
    );
    parameters.addParameter(velocityFix);

    DiscreteRangeParameter* velocityMin = new DiscreteRangeParameter(
        STR16("Velocity Min"),  // title
        PARAM_ID_VELOCITY_MIN,  // tag
        nullptr,  // units
        127,  // step count
        0,  // min plain
        127,  // max plain
        DEFAULT_VELOCITY  // default value plain
    );
    parameters.addParameter(velocityMin);

    DiscreteRangeParameter* velocityMax = new DiscreteRangeParameter(
        STR16("Velocity Max"),  // title
        PARAM_ID_VELOCITY_MAX,  // tag
        nullptr,  // units
        127,  // step count
        0,  // min plain
        127,  // max plain
        DEFAULT_VELOCITY  // default value plain
    );
    parameters.addParameter(velocityMax);

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