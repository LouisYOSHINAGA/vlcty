#include "guieditor.h"
#include "config.h"
#include <stdexcept>


namespace Steinberg {
namespace Vst {


VelocityGUIEditor::VelocityGUIEditor(EditController* controller)
    : VSTGUIEditor(controller)
    , sliders{}, sliderLabels{}, textEdits{}{
    ViewRect viewRect(0, 0, this->backgroundWidth, this->backgroundHeight);
    setRect(viewRect);
}

bool PLUGIN_API VelocityGUIEditor::open(void* parent, const PlatformType& platformType){
    if(frame){  // frame is already created
        return false;
    }

    CRect size(0, 0, this->backgroundWidth, this->backgroundHeight);  // frame size
    frame = new CFrame(size, this);  // create frame
    if(frame == nullptr){
        return false;
    }

    // background image
    CBitmap* cbmp = new CBitmap("background.png");
    frame->setBackground(cbmp);
    frame->open(parent);
    cbmp->forget();  // release background image

    // controls
    this->createLabel(MYVST_VSTNAME, 25, 0, 150, 45, false, this->defaultFontsize, true);
    this->createCombobox(PARAM_ID_TYPE, correctTypeNames, 200, 10, 80, 30,
                         this->defaultFontsize, false, kCenterText);

    this->sliders[SLIDER_ID_VELOCITY_FIX] = this->createHorizontalSlider(PARAM_ID_VELOCITY_FIX, 20, 45, false);
    this->sliders[SLIDER_ID_VELOCITY_MAX] = this->createHorizontalSlider(PARAM_ID_VELOCITY_MAX, 20, 90, true);
    this->sliders[SLIDER_ID_VELOCITY_MIN] = this->createHorizontalSlider(PARAM_ID_VELOCITY_MIN, 20, 135, true);

    this->sliderLabels[SLIDER_ID_VELOCITY_FIX] = this->createLabel(
        std::string(this->sliderNames[SLIDER_ID_VELOCITY_FIX]) + ":", 300, 45, 50, 40, false
    );
    this->sliderLabels[SLIDER_ID_VELOCITY_MIN] = this->createLabel(
        std::string(this->sliderNames[SLIDER_ID_VELOCITY_MIN]) + ":", 300, 90, 50, 40, true
    );
    this->sliderLabels[SLIDER_ID_VELOCITY_MAX] = this->createLabel(
        std::string(this->sliderNames[SLIDER_ID_VELOCITY_MAX]) + ":", 300, 135, 50, 40, true
    );

    this->textEdits[SLIDER_ID_VELOCITY_FIX] = this->createTextEdit(PARAM_ID_VELOCITY_FIX, 350, 45, 50, 40, false);
    this->textEdits[SLIDER_ID_VELOCITY_MAX] = this->createTextEdit(PARAM_ID_VELOCITY_MAX, 350, 90, 50, 40, true);
    this->textEdits[SLIDER_ID_VELOCITY_MIN] = this->createTextEdit(PARAM_ID_VELOCITY_MIN, 350, 135, 50, 40, true);
    for(int8 i = 0; i < N_SLIDERS; i++){
        this->textEdits[i]->setStringToValueFunction(&VelocityGUIEditor::parseInputText);
    }

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

    switch(paramId){
        case PARAM_ID_TYPE:
            bool isLockFix, isLockMin, isLockMax;
            if(static_cast<CorrectTypeID>(index) == CORRECT_TYPE_FIX){  // fix
                isLockFix = false;
                isLockMin = isLockMax = true;
            }else{  // remap, clip
                isLockFix = true;
                isLockMin = isLockMax = false;
            }

            this->lockHorizontalSlider(this->sliders[SLIDER_ID_VELOCITY_FIX], isLockFix);
            this->lockHorizontalSlider(this->sliders[SLIDER_ID_VELOCITY_MIN], isLockMin);
            this->lockHorizontalSlider(this->sliders[SLIDER_ID_VELOCITY_MAX], isLockMax);

            this->lockLabel(this->sliderLabels[SLIDER_ID_VELOCITY_FIX], isLockFix);
            this->lockLabel(this->sliderLabels[SLIDER_ID_VELOCITY_MIN], isLockMin);
            this->lockLabel(this->sliderLabels[SLIDER_ID_VELOCITY_MAX], isLockMax);

            this->lockTextEdit(this->textEdits[SLIDER_ID_VELOCITY_FIX], isLockFix);
            this->lockTextEdit(this->textEdits[SLIDER_ID_VELOCITY_MIN], isLockMin);
            this->lockTextEdit(this->textEdits[SLIDER_ID_VELOCITY_MAX], isLockMax);
            break;

        case PARAM_ID_VELOCITY_FIX:
            this->textEdits[SLIDER_ID_VELOCITY_FIX]->setText(
                std::to_string(static_cast<uint8>(value * MAX_VELOCITY)).c_str()
            );
            this->sliders[SLIDER_ID_VELOCITY_FIX]->setValueNormalized(value);
            break;

        case PARAM_ID_VELOCITY_MIN:
            this->textEdits[SLIDER_ID_VELOCITY_MIN]->setText(
                std::to_string(static_cast<uint8>(value * MAX_VELOCITY)).c_str()
            );
            this->sliders[SLIDER_ID_VELOCITY_MIN]->setValueNormalized(value);
            break;

        case PARAM_ID_VELOCITY_MAX:
            this->textEdits[SLIDER_ID_VELOCITY_MAX]->setText(
                std::to_string(static_cast<uint8>(value * MAX_VELOCITY)).c_str()
            );
            this->sliders[SLIDER_ID_VELOCITY_MAX]->setValueNormalized(value);
            break;

        default:
            // do nothing
            break;
    }

    this->controller->setParamNormalized(paramId, value);
    this->controller->performEdit(paramId, value);
}

CTextLabel* VelocityGUIEditor::createLabel(std::string text, uint16 x, uint16 y, uint16 w, uint16 h,
                                           bool isLock, uint8 fontsize, bool isBold, CHoriTxtAlign align){
    return this->createLabel(text.c_str(), x, y, w, h, isLock, fontsize, isBold, align);
}

CTextLabel* VelocityGUIEditor::createLabel(UTF8StringPtr text, uint16 x, uint16 y, uint16 w, uint16 h,
                                           bool isLock, uint8 fontsize, bool isBold, CHoriTxtAlign align){
    CRect size(0, 0, w, h);
    size.offset(x, y);

    CFontDesc* font = new CFontDesc(this->defaultFontName, fontsize, isBold? kBoldFace: kNormalFace);
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

    if(isLock){
        this->lockLabel(label, true);
    }

    return label;
}

void VelocityGUIEditor::lockLabel(CTextLabel* label, bool isLock){
    label->setAlphaValue(isLock? 0.2: 1.0);
}

COptionMenu* VelocityGUIEditor::createCombobox(ParamID tag, std::vector<UTF8StringPtr> items,
                                               uint16 x, uint16 y, uint16 w, uint16 h,
                                               uint8 fontsize, bool isBold, CHoriTxtAlign align){
    CRect size(0, 0, w, h);
    size.offset(x, y);

    CFontDesc* font = new CFontDesc(this->defaultFontName, fontsize, isBold? kBoldFace: kNormalFace);
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

    CPoint trackOffset(0, -(sliderHandle->getHeight()-sliderBackground->getHeight())/2);
    CHorizontalSlider* slider = new CHorizontalSlider(size, this, tag,
                                                      x, x+sliderBackground->getWidth()-sliderHandle->getWidth(),
                                                      sliderHandle, sliderBackground);

    slider->setStyle(CSlider::kLeft | CSlider::kHorizontal);
    slider->setBackgroundOffset(trackOffset);
    slider->setValueNormalized(this->controller->getParamNormalized(tag));

    frame->addView(slider);

    if(isLock){
        this->lockHorizontalSlider(slider, true);
    }

    sliderBackground->forget();
    sliderHandle->forget();

    return slider;
}

void VelocityGUIEditor::lockHorizontalSlider(CHorizontalSlider* slider, bool isLock){
    slider->setMouseEnabled(!isLock);
    slider->setAlphaValue(isLock? 0.2: 1.0);
}

CTextEdit* VelocityGUIEditor::createTextEdit(ParamID tag, uint16 x, uint16 y, uint16 w, uint16 h,
                                             bool isLock, uint8 fontsize, bool isBold, CHoriTxtAlign align){
    CRect size(0, 0, w, h);
    size.offset(x, y);

    CFontDesc* font = new CFontDesc(this->defaultFontName, fontsize, isBold? kBoldFace: kNormalFace);
    CTextEdit* textEdit = new CTextEdit(size, this, tag);

    textEdit->setFont(font);
    textEdit->setHoriAlign(align);
    textEdit->setBackColor(this->defaultBackgroundColor);
    #ifdef DEBUG_GUIEDITOR
        textEdit->setFrameColor(kWhiteCColor);
    #else
        textEdit->setFrameColor(kTransparentCColor);
    #endif
    textEdit->setText(
        std::string(
            std::to_string(static_cast<uint8>(this->controller->getParamNormalized(tag) * MAX_VELOCITY))
        ).c_str()
    );

    frame->addView(textEdit);

    if(isLock){
        this->lockTextEdit(textEdit, true);
    }

    return textEdit;
}

void VelocityGUIEditor::lockTextEdit(CTextEdit* textEdit, bool isLock){
    textEdit->setMouseEnabled(!isLock);
    textEdit->setAlphaValue(isLock? 0.2: 1.0);
}

bool VelocityGUIEditor::parseInputText(UTF8StringPtr inputText, float& value, CTextEdit* textEdit){
    try{
        size_t idx;
        float inputValue = std::stof(std::string(inputText), &idx);
        if(idx != std::string(inputText).length()){
            throw std::invalid_argument("invalid input");
        }else if(inputValue < MIN_VELOCITY || MAX_VELOCITY < inputValue){
            throw std::out_of_range("out of range");
        }

        value = inputValue / MAX_VELOCITY;
    }catch(...){
        return false;
    }

    return true;
}


} }