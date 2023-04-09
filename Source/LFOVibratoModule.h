#pragma once
#include <JuceHeader.h>

class LFOVibratoModule: public juce::Component {
    public:
        LFOVibratoModule();
        ~LFOVibratoModule() override;

        void paint (juce::Graphics&) override;
        void resized() override;

    private:
//        juce::TabbedButtonBar tabSelectBar;
        
        juce::Slider lfoAmountSlider, lfoRateSlider;
        juce::ComboBox lfoShapeMenu;
    
        juce::Slider vibratoAmountSlider, vibratoRateSlider;
        juce::ComboBox vibratoShapeMenu;
    
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOVibratoModule)
};
