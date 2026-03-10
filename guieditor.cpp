#include "guieditor.h"


namespace Steinberg {
namespace Vst {


VelocityGUIEditor::VelocityGUIEditor(EditController* controller):
    VSTGUIEditor(controller){
    ViewRect viewRect(0, 0, this->BG_WIDTH, this->BG_HEIGHT);
    setRect(viewRect);
}

bool PLUGIN_API VelocityGUIEditor::open(void* parent, const PlatformType& platformType){
    if(frame){  // frame is already created
        return false;
    }

    CRect size(0, 0, this->BG_WIDTH, this->BG_HEIGHT);  // configure frame size
    frame = new CFrame(size, this);  // create frame
    if(frame == nullptr){
        return false;
    }

    // background image
    CBitmap* cbmp = new CBitmap("background.png");
    frame->setBackground(cbmp);
    cbmp->forget();  // release background image
    frame->open(parent);

    // this->createSlider(PARAM_ID_VELOCITY, xxx, xxx);

    return true;
}

void PLUGIN_API VelocityGUIEditor::close(){
    if(frame){
        // release frame
        frame->forget();
        frame = 0;
    }
}

void VelocityGUIEditor::valueChanged(CControl* pControl){
    // int32 index = control->getTag();
    // float value = control->getValueNormalized();
    // controller->setParamNormalized(index, value);
    // controller->performEdit(index, value);
}

void VelocityGUIEditor::createSlider(ParamID tag, uint16 x, uint16 y){
    // TODO impl
}


} }