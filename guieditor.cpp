#include "guieditor.h"
#include "config.h"
#include <stdexcept>


namespace Steinberg {
namespace Vst {


VelocityGUIEditor::CContextMenu::CContextMenu(const CRect& size, VelocityGUIEditor* velocityGUIEditor)
    : CView(size)
    , velocityGUIEditor(velocityGUIEditor){
}

CMouseEventResult VelocityGUIEditor::CContextMenu::onMouseDown(CPoint& point, const CButtonState& buttons){
    if(buttons.isRightButton()){
        // generate zoom factor menu
        COptionMenu* menu = new COptionMenu(CRect(0, 0, 0, 0), nullptr, -1);
        menu->setStyle(COptionMenu::kPopupStyle);
        for(float zoomFactor = this->minZoomFactor; zoomFactor <= this->maxZoomFactor; zoomFactor += this->zoomFactorStep){
            menu->addEntry(std::to_string(static_cast<uint16>(100*zoomFactor)) + "%");
        }
        menu->popup(this->velocityGUIEditor->getFrame(), point);

        // apply selected zoom factor
        float zoomFactor = this->minZoomFactor + this->zoomFactorStep * static_cast<uint8>(menu->getValue());
        this->velocityGUIEditor->getFrame()->setZoom(zoomFactor);
        this->velocityGUIEditor->requestResize(zoomFactor);

        menu->forget();
        return kMouseEventHandled;
    }

    return kMouseEventNotHandled;
}

VelocityGUIEditor::VelocityGUIEditor(EditController* controller)
    : VSTGUIEditor(controller)
    , currentInputVelocity(DEFAULT_VELOCITY), currentOutputVelocity(DEFAULT_VELOCITY)
    , contextMenu(nullptr), velocityLabel(nullptr)
    , sliders{}, sliderLabels{}, textEdits{}{
    ViewRect viewRect(0, 0, this->backgroundWidth, this->backgroundHeight);
    setRect(viewRect);
}

bool PLUGIN_API VelocityGUIEditor::open(void* parent, const PlatformType& platformType){
    if(this->frame){  // frame is already created
        return false;
    }

    // frame
    CRect size(0, 0, this->backgroundWidth, this->backgroundHeight);  // frame size
    this->frame = new CFrame(size, this);  // create frame
    if(this->frame == nullptr){
        return false;
    }

    // background
    CBitmap* bitmap = new CBitmap("background.png");
    CView* bgview = new CView(size);
    bgview->setBackground(bitmap);  // set background image to view
    this->frame->addView(bgview);
    bitmap->forget();  // release background image

    // controls
    this->contextMenu = this->createContextMenu(this->backgroundWidth, this->backgroundHeight);
    this->createLabel(MYVST_VSTNAME, 30, 0, 130, 45, this->defaultFontsize+2, true, kCenterText);
    this->createCombobox(PARAM_ID_CORRECT_TYPE, correctTypeNames, 170, 7.5, 90, 30,
                         this->defaultFontsize, false, kCenterText);
    this->velocityLabel = this->createLabel("    ->    ", 270, 0, 110, 45, this->defaultFontsize, false, kCenterText);

    this->sliders[SLIDER_ID_VELOCITY_FIX] = this->createHorizontalSlider(PARAM_ID_VELOCITY_FIX, 25, 45);
    this->sliders[SLIDER_ID_VELOCITY_MIN] = this->createHorizontalSlider(PARAM_ID_VELOCITY_MIN, 25, 90);
    this->sliders[SLIDER_ID_VELOCITY_MAX] = this->createHorizontalSlider(PARAM_ID_VELOCITY_MAX, 25, 135);

    this->sliderLabels[SLIDER_ID_VELOCITY_FIX] = this->createLabel(
        std::string(this->sliderNames[SLIDER_ID_VELOCITY_FIX]) + ":", 305, 45, 40, 40
    );
    this->sliderLabels[SLIDER_ID_VELOCITY_MIN] = this->createLabel(
        std::string(this->sliderNames[SLIDER_ID_VELOCITY_MIN]) + ":", 305, 90, 40, 40
    );
    this->sliderLabels[SLIDER_ID_VELOCITY_MAX] = this->createLabel(
        std::string(this->sliderNames[SLIDER_ID_VELOCITY_MAX]) + ":", 305, 135, 40, 40
    );

    this->textEdits[SLIDER_ID_VELOCITY_FIX] = this->createTextEdit(PARAM_ID_VELOCITY_FIX, 350, 45, 40, 40);
    this->textEdits[SLIDER_ID_VELOCITY_MIN] = this->createTextEdit(PARAM_ID_VELOCITY_MIN, 350, 90, 40, 40);
    this->textEdits[SLIDER_ID_VELOCITY_MAX] = this->createTextEdit(PARAM_ID_VELOCITY_MAX, 350, 135, 40, 40);
    for(int8 i = 0; i < N_SLIDERS; i++){
        this->textEdits[i]->setStringToValueFunction(&VelocityGUIEditor::parseInputText);
    }

    this->lockControls(this->controller->getParamNormalized(PARAM_ID_CORRECT_TYPE));

    this->frame->open(parent);  // register frame to DAW window

    return true;
}

void PLUGIN_API VelocityGUIEditor::close(){
    if(this->frame){  // release frame
        this->frame->forget();
        this->frame = 0;
    }
    static_cast<VelocityController*>(this->controller.get())->closeView();  // notify controller that GUI is closed
}

void VelocityGUIEditor::valueChanged(CControl* control){
    int8 paramId = control->getTag();
    float value = control->getValueNormalized();

    switch(paramId){
        case PARAM_ID_CORRECT_TYPE:
            this->lockControls(value);
            break;

        case PARAM_ID_VELOCITY_FIX:
            this->textEdits[SLIDER_ID_VELOCITY_FIX]->setText(
                std::to_string(static_cast<uint8>(value * MAX_VELOCITY + EPSILON)).c_str()
            );
            this->sliders[SLIDER_ID_VELOCITY_FIX]->setValueNormalized(value);
            break;

        case PARAM_ID_VELOCITY_MIN:
            this->textEdits[SLIDER_ID_VELOCITY_MIN]->setText(
                std::to_string(static_cast<uint8>(value * MAX_VELOCITY + EPSILON)).c_str()
            );
            this->sliders[SLIDER_ID_VELOCITY_MIN]->setValueNormalized(value);
            break;

        case PARAM_ID_VELOCITY_MAX:
            this->textEdits[SLIDER_ID_VELOCITY_MAX]->setText(
                std::to_string(static_cast<uint8>(value * MAX_VELOCITY + EPSILON)).c_str()
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

bool VelocityGUIEditor::requestResize(float zoom){
    if(this->plugFrame == nullptr){
        return false;
    }

    ViewRect newRect(0, 0, zoom*this->backgroundWidth, zoom*this->backgroundHeight);
    this->getFrame()->setZoom(zoom);

    return this->plugFrame->resizeView(this, &newRect) == kResultTrue;
}

void VelocityGUIEditor::updateVelocityLabel(ParamValue rawVelocity, bool isInput){
    if(this->velocityLabel == nullptr){
        return;
    }

    uint8 velocity = static_cast<uint8>(rawVelocity * MAX_VELOCITY + EPSILON);
    if(isInput){
        this->currentInputVelocity = velocity;
    }else{
        this->currentOutputVelocity = velocity;
    }

    char labelText[16];
    snprintf(labelText, sizeof(labelText), "%3d -> %3d",
             this->currentInputVelocity, this->currentOutputVelocity);
    this->velocityLabel->setText(labelText);
    this->velocityLabel->invalid();
}

VelocityGUIEditor::CContextMenu* VelocityGUIEditor::createContextMenu(uint16 w, uint16 h){
    CRect size(0, 0, w, h);
    CContextMenu* contextMenu = new CContextMenu(size, this);

    this->frame->addView(contextMenu);

    return contextMenu;
}

CTextLabel* VelocityGUIEditor::createLabel(std::string text, uint16 x, uint16 y, uint16 w, uint16 h,
                                           uint8 fontsize, bool isBold, CHoriTxtAlign align){
    return this->createLabel(text.c_str(), x, y, w, h, fontsize, isBold, align);
}

CTextLabel* VelocityGUIEditor::createLabel(UTF8StringPtr text, uint16 x, uint16 y, uint16 w, uint16 h,
                                           uint8 fontsize, bool isBold, CHoriTxtAlign align){
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

    this->frame->addView(label);

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

    this->frame->addView(combobox);

    return combobox;
}

CHorizontalSlider* VelocityGUIEditor::createHorizontalSlider(ParamID tag, uint16 x, uint16 y){
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

    this->frame->addView(slider);

    sliderBackground->forget();
    sliderHandle->forget();

    return slider;
}

void VelocityGUIEditor::lockHorizontalSlider(CHorizontalSlider* slider, bool isLock){
    slider->setMouseEnabled(!isLock);
    slider->setAlphaValue(isLock? 0.2: 1.0);
}

CTextEdit* VelocityGUIEditor::createTextEdit(ParamID tag, uint16 x, uint16 y, uint16 w, uint16 h,
                                             uint8 fontsize, bool isBold, CHoriTxtAlign align){
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
        std::to_string(
            static_cast<uint8>(this->controller->getParamNormalized(tag) * MAX_VELOCITY + EPSILON)
        ).c_str()
    );

    this->frame->addView(textEdit);

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

void VelocityGUIEditor::lockControls(ParamValue value){
    CorrectTypeID correctType = static_cast<CorrectTypeID>(value * (N_CORRECT_TYPES - 1) + EPSILON);
    bool isLockFix, isLockMin, isLockMax;

    switch(correctType){
        case CORRECT_TYPE_THROUGH:
            isLockFix = isLockMin = isLockMax = true;
            break;
        case CORRECT_TYPE_FIX:
            isLockFix = false;
            isLockMin = isLockMax = true;
            break;
        case CORRECT_TYPE_REMAP:
        case CORRECT_TYPE_CLIP:
            isLockFix = true;
            isLockMin = isLockMax = false;
            break;
        default:  // never reached
            isLockFix = isLockMin = isLockMax = true;
            break;
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
}


} }