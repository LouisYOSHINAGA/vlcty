#include "guieditor.h"


namespace Steinberg {
namespace Vst {


VelocityGUIEditor::VelocityGUIEditor(EditController* controller):
    VSTGUIEditor(controller){
    ViewRect viewRect(0, 0, this->BG_WIDTH, this->BG_HEIGHT);
    setRect(viewRect);
}

bool PLUGIN_API VelocityGUIEditor::open(void* parent, const PlatformType& platformType=PlatformType::kDefaultNative){
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

    this->createHorizontalSlider(PARAM_ID_VELOCITY, 30, 30);

    return true;
}

void PLUGIN_API VelocityGUIEditor::close(){
    if(frame){
        // release frame
        frame->forget();
        frame = 0;
    }
}

void VelocityGUIEditor::valueChanged(CControl* control){
    int32 index = control->getTag();
    float value = control->getValueNormalized();
    this->controller->setParamNormalized(index, value);
    this->controller->performEdit(index, value);
}

void VelocityGUIEditor::createHorizontalSlider(ParamID tag, uint16 x, uint16 y){
    CBitmap* slider_track = new CBitmap("slider_track.png");
    CBitmap* slider_handle = new CBitmap("slider_handle.png");

    CRect size(0, 0, slider_track->getWidth(), slider_handle->getHeight());
    size.offset(x, y);


    CHorizontalSlider* slider = new CHorizontalSlider(size, this, tag,
                                                      x, x+slider_track->getWidth()-slider_handle->getWidth(),
                                                      slider_handle, slider_track);
    CPoint track_offset(0, -(slider_handle->getHeight()-slider_track->getHeight())/2);
    slider->setBackgroundOffset(track_offset);
    slider->setValueNormalized(this->controller->getParamNormalized(tag));
    frame->addView(slider);

    slider_track->forget();
    slider_handle->forget();
}


} }