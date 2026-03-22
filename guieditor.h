#pragma once

#include "public.sdk/source/vst/vstguieditor.h"
#include "pluginterfaces/vst/ivstplugview.h"
#include "param.h"
#include <string>


// #define DEBUG_GUIEDITOR


namespace Steinberg {
namespace Vst {
using namespace VSTGUI;


class VelocityGUIEditor : public VSTGUIEditor
                        , public IControlListener {

    class CContextMenu : public CView {
        public:
            CContextMenu(const CRect&, VelocityGUIEditor*);
            CMouseEventResult onMouseDown(CPoint&, const CButtonState&) override;
        protected:
            static constexpr float minZoomFactor = 1.0;
            static constexpr float maxZoomFactor = 2.0;
            static constexpr float zoomFactorStep = 0.25;
            VelocityGUIEditor* velocityGUIEditor;
    };

    public:
        VelocityGUIEditor(EditController*);
        virtual bool PLUGIN_API open(void*, const PlatformType& = PlatformType::kDefaultNative);
        virtual void PLUGIN_API close();
        void valueChanged(CControl*);
        void updateVelocityLabel(ParamValue, bool);
        DELEGATE_REFCOUNT(VSTGUIEditor)

    protected:
        // constants
        static constexpr uint16 backgroundWidth = 400;
        static constexpr uint16 backgroundHeight= 185;
        static constexpr UTF8StringPtr defaultFontName = "Consolas";
        static constexpr uint8 defaultFontsize = 18;
        static constexpr CColor defaultBackgroundColor = CColor(91, 155, 213, 255);

        enum SliderID {
            SLIDER_ID_VELOCITY_FIX,
            SLIDER_ID_VELOCITY_MIN,
            SLIDER_ID_VELOCITY_MAX,
            N_SLIDERS
        };
        std::vector<VSTGUI::UTF8StringPtr> sliderNames = {
            "Fix",
            "Min",
            "Max"
        };

        // cache for velocity label
        uint8 currentInputVelocity;
        uint8 currentOutputVelocity;

        // pointers to controls
        CContextMenu* contextMenu;
        CTextLabel* velocityLabel;
        CHorizontalSlider* sliders[N_SLIDERS];
        CTextLabel* sliderLabels[N_SLIDERS];
        CTextEdit* textEdits[N_SLIDERS];

        // context menu
        VelocityGUIEditor::CContextMenu* createContextMenu(uint16, uint16);
        bool requestResize(float);

        // label
        CTextLabel* createLabel(std::string, uint16, uint16, uint16, uint16,
                                bool = true, uint8 = defaultFontsize,
                                bool = false, CHoriTxtAlign = kLeftText);
        CTextLabel* createLabel(UTF8StringPtr, uint16, uint16, uint16, uint16,
                                bool = true, uint8 = defaultFontsize,
                                bool = false, CHoriTxtAlign = kLeftText);
        void lockLabel(CTextLabel*, bool = true);

        // combobox
        COptionMenu* createCombobox(ParamID, std::vector<UTF8StringPtr>,
                                    uint16, uint16, uint16, uint16,
                                    uint8 = defaultFontsize, bool = false,
                                    CHoriTxtAlign = kLeftText);

        // horizontal slider
        CHorizontalSlider* createHorizontalSlider(ParamID, uint16, uint16, bool = true);
        void lockHorizontalSlider(CHorizontalSlider*, bool = true);

        // text edit
        CTextEdit* createTextEdit(ParamID, uint16, uint16, uint16, uint16,
                                  bool = true, uint8 = defaultFontsize,
                                  bool = false, CHoriTxtAlign = kLeftText);
        void lockTextEdit(CTextEdit*, bool = true);
        static bool parseInputText(UTF8StringPtr, float&, CTextEdit*);
};


} }