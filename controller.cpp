#include "controller.h"
#include "config.h"


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

    StringListParameter* correctType = new StringListParameter(
        STR16("Correct Type"),  // title
        PARAM_ID_CORRECT_TYPE  // tag
    );
    correctType->appendString(STR16("Fix"));
    correctType->appendString(STR16("Remap"));
    correctType->appendString(STR16("Clip"));
    parameters.addParameter(correctType);

    VelocityStepParameter* velocityFix = new VelocityStepParameter(
        STR16("Velocity Fix"),  // title
        PARAM_ID_VELOCITY_FIX  // tag
    );
    parameters.addParameter(velocityFix);

    VelocityStepParameter* velocityMin = new VelocityStepParameter(
        STR16("Velocity Min"),  // title
        PARAM_ID_VELOCITY_MIN  // tag
    );
    parameters.addParameter(velocityMin);

    VelocityStepParameter* velocityMax = new VelocityStepParameter(
        STR16("Velocity Max"),  // title
        PARAM_ID_VELOCITY_MAX  // tag
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