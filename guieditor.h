#pragma once

#include "public.sdk/source/vst/vstguieditor.h"
#include "pluginterfaces/vst/ivstplugview.h"
#include "param.h"
#include <string>


// #define DEBUG_GUIEDITOR


namespace Steinberg {
namespace Vst {
using namespace VSTGUI;


class VelocityGUIEditor : public VSTGUIEditor,
                          public IControlListener {
    protected:
        static constexpr uint16 BG_WIDTH = 400;
        static constexpr uint16 BG_HEIGHT = 185;
        static constexpr uint8 DEFAULT_FONTSIZE = 18;

        enum{
            SLIDER_ID_VELOCITY_FIX,
            SLIDER_ID_VELOCITY_MIN,
            SLIDER_ID_VELOCITY_MAX,
            N_SLIDERS
        } SliderID;
        std::vector<VSTGUI::UTF8StringPtr> sliderNames = {
            "Fix",
            "Min",
            "Max"
        };


        CHorizontalSlider* sliders[N_SLIDERS];
        CTextLabel* sliderLabels[N_SLIDERS];
        CTextEdit* textEdits[N_SLIDERS];

        CTextLabel* createLabel(std::string, uint16, uint16, uint16, uint16,
                                uint8 = DEFAULT_FONTSIZE, bool = false, CHoriTxtAlign = kLeftText);
        CTextLabel* createLabel(UTF8StringPtr, uint16, uint16, uint16, uint16,
                                uint8 = DEFAULT_FONTSIZE, bool = false, CHoriTxtAlign = kLeftText);
        void lockLabel(CTextLabel*, bool = true);
        COptionMenu* createCombobox(ParamID, std::vector<UTF8StringPtr>, uint16, uint16, uint16, uint16,
                                    uint8 = DEFAULT_FONTSIZE, bool = false, CHoriTxtAlign = kLeftText);
        CHorizontalSlider* createHorizontalSlider(ParamID, uint16, uint16, bool = false);
        void lockHorizontalSlider(CHorizontalSlider*, bool = true);
        // CTextEdit* createTextEdit(ParamID, uint16, uint16, uint16, uint16,
        //                           uint8 = DEFAULT_FONTSIZE, bool = false, CHoriTxtAlign = kLeftText);
    public:
        VelocityGUIEditor(EditController*);
        virtual bool PLUGIN_API open(void*, const PlatformType& = PlatformType::kDefaultNative);
        virtual void PLUGIN_API close();
        void valueChanged(CControl*);
        DELEGATE_REFCOUNT(VSTGUIEditor)
};


} }