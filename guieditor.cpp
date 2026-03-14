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
    this->createHorizontalSlider(PARAM_ID_VELOCITY, 30, 45);
    this->createHorizontalSlider(PARAM_ID_VELOCITY, 30, 90);  // dummy
    this->createHorizontalSlider(PARAM_ID_VELOCITY, 30, 135);  // dummy

    return true;
}

void PLUGIN_API VelocityGUIEditor::close(){
    if(frame){  // release frame
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
}

void VelocityGUIEditor::createCombobox(ParamID tag, std::vector<UTF8StringPtr> items,
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
}

void VelocityGUIEditor::createHorizontalSlider(ParamID tag, uint16 x, uint16 y){
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
}


} }