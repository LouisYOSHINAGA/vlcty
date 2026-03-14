#include "guieditor.h"
#include "config.h"


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

    this->createLabel(MYVST_VSTNAME, 30, 0, 150, 45, 18, true);
    this->createHorizontalSlider(PARAM_ID_VELOCITY, 30, 45);
    this->createHorizontalSlider(PARAM_ID_VELOCITY, 30, 90);  // dummy
    this->createHorizontalSlider(PARAM_ID_VELOCITY, 30, 135);  // dummy

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

void VelocityGUIEditor::createLabel(UTF8StringPtr text, uint16 x, uint16 y, uint16 w, uint16 h,
                                    uint8 fontsize, bool isBold, CHoriTxtAlign align){
    CRect size(0, 0, w, h);
    size.offset(x, y);

    CFontDesc* font = new CFontDesc("Consolas", fontsize,
                                    isBold ? kBoldFace : kNormalFace);
    CTextLabel* label = new CTextLabel(size, text);

    label->setFont(font);
    label->setHoriAlign(align);
    label->setBackColor(kTransparentCColor);
    #ifdef DEBUG_GUIEDITOR
        label->setFrameColor(kWhiteCColor);
    #else
        label->setFrameColor(kTransparentCColor);
    #endif

    frame->addView(label);
}

void VelocityGUIEditor::createHorizontalSlider(ParamID tag, uint16 x, uint16 y){
    CBitmap* slider_background = new CBitmap("slider_background.png");
    CBitmap* slider_handle = new CBitmap("slider_handle.png");

    CRect size(0, 0, slider_background->getWidth(), slider_handle->getHeight());
    size.offset(x, y);

    CHorizontalSlider* slider = new CHorizontalSlider(size, this, tag,
                                                      x, x+slider_background->getWidth()-slider_handle->getWidth(),
                                                      slider_handle, slider_background);
    slider->setStyle(CSlider::kLeft | CSlider::kHorizontal);

    CPoint track_offset(0, -(slider_handle->getHeight()-slider_background->getHeight())/2);
    slider->setBackgroundOffset(track_offset);

    slider->setValueNormalized(this->controller->getParamNormalized(tag));
    frame->addView(slider);

    slider_background->forget();
    slider_handle->forget();
}


} }