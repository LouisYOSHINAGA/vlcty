#include "guieditor.h"
#include "config.h"


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
    frame->open(parent);
    cbmp->forget();  // release background image

    this->createLabel(MYVST_VSTNAME, 30, 0, 150, 45, 18, true);
    this->createCombobox(PARAM_ID_TYPE, correctTypeNames, 200, 10, 80, 30, 18, false, kCenterText);
    this->sliders[SLIDER_ID_VELOCITY_FIX] = this->createHorizontalSlider(PARAM_ID_VELOCITY_FIX, 30, 45, false);
    this->sliders[SLIDER_ID_VELOCITY_MAX] = this->createHorizontalSlider(PARAM_ID_VELOCITY_MAX, 30, 90, true);
    this->sliders[SLIDER_ID_VELOCITY_MIN] = this->createHorizontalSlider(PARAM_ID_VELOCITY_MIN, 30, 135, true);

    return true;
}

void PLUGIN_API VelocityGUIEditor::close(){
    if(frame){  // release frame
        frame->forget();
        frame = 0;
    }
}

void VelocityGUIEditor::valueChanged(CControl* control){
    int8 paramId = control->getTag();
    int8 index = control->getValue();
    float value = control->getValueNormalized();

    if(paramId == PARAM_ID_TYPE){
        if(static_cast<CorrectTypeID>(index) == CORRECT_TYPE_FIX){
            this->lockSlider(this->sliders[SLIDER_ID_VELOCITY_FIX], false);
            this->lockSlider(this->sliders[SLIDER_ID_VELOCITY_MIN], true);
            this->lockSlider(this->sliders[SLIDER_ID_VELOCITY_MAX], true);
        }else{
            this->lockSlider(this->sliders[SLIDER_ID_VELOCITY_FIX], true);
            this->lockSlider(this->sliders[SLIDER_ID_VELOCITY_MIN], false);
            this->lockSlider(this->sliders[SLIDER_ID_VELOCITY_MAX], false);
        }
    }

    this->controller->setParamNormalized(paramId, value);
    this->controller->performEdit(paramId, value);
}

void VelocityGUIEditor::lockSlider(CHorizontalSlider* slider, bool isLock){
    slider->setMouseEnabled(!isLock);
    slider->setAlphaValue(isLock? 0.2: 1.0);
}

CTextLabel* VelocityGUIEditor::createLabel(UTF8StringPtr text, uint16 x, uint16 y, uint16 w, uint16 h,
                                    uint8 fontsize, bool isBold, CHoriTxtAlign align){
    CRect size(0, 0, w, h);
    size.offset(x, y);

    CFontDesc* font = new CFontDesc("Consolas", fontsize, isBold? kBoldFace: kNormalFace);
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

    return label;
}

COptionMenu* VelocityGUIEditor::createCombobox(ParamID tag, std::vector<UTF8StringPtr> items,
                                               uint16 x, uint16 y, uint16 w, uint16 h,
                                               uint8 fontsize, bool isBold, CHoriTxtAlign align){
    CRect size(0, 0, w, h);
    size.offset(x, y);

    CFontDesc* font = new CFontDesc("Consolas", fontsize, isBold? kBoldFace: kNormalFace);
    COptionMenu* combobox = new COptionMenu(size, this, tag);

    for(uint8 i = 0; i < items.size(); i++){
        combobox->addEntry(items[i]);
    }
    combobox->setFont(font);
    combobox->setHoriAlign(align);
    combobox->setBackColor(kTransparentCColor);
    combobox->setFrameColor(kWhiteCColor);
    combobox->setValueNormalized(this->controller->getParamNormalized(tag));

    frame->addView(combobox);

    return combobox;
}

CHorizontalSlider* VelocityGUIEditor::createHorizontalSlider(ParamID tag, uint16 x, uint16 y, bool isLock){
    CBitmap* sliderBackground = new CBitmap("sliderBackground.png");
    CBitmap* sliderHandle = new CBitmap("sliderHandle.png");

    CRect size(0, 0, sliderBackground->getWidth(), sliderHandle->getHeight());
    size.offset(x, y);

    CHorizontalSlider* slider = new CHorizontalSlider(size, this, tag,
                                                      x, x+sliderBackground->getWidth()-sliderHandle->getWidth(),
                                                      sliderHandle, sliderBackground);
    slider->setStyle(CSlider::kLeft | CSlider::kHorizontal);

    CPoint trackOffset(0, -(sliderHandle->getHeight()-sliderBackground->getHeight())/2);
    slider->setBackgroundOffset(trackOffset);

    slider->setValueNormalized(this->controller->getParamNormalized(tag));
    frame->addView(slider);

    sliderBackground->forget();
    sliderHandle->forget();

    if(isLock){
        this->lockSlider(slider, true);
    }

    return slider;
}


} }