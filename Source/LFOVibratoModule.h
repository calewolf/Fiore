#pragma once
#include <JuceHeader.h>

class LFOTab: public juce::Component {
    public:
        LFOTab(const String& title);
        ~LFOTab();
        void paint (juce::Graphics&) override;
        void resized() override;
    private:
        enum RadioGroupID {
            group1 = 1001
        };
        juce::Slider lfoAmountSlider, lfoRateSlider;
        juce::Label lfoAmountSliderLabel, lfoRateSliderLabel;
    
        juce::Label lfoShapeLabel;
        juce::ToggleButton sawUpButton {"Saw (asc)"},
                           sawDownButton {"Saw (desc)"},
                           triangleButton {"Triangle"},
                           squareButton {"Square"},
                           noiseButton {"Noise"};
    
        juce::Label lfoTabLabel;
};

class LFOVibratoModule: public juce::Component {
    public:
        LFOVibratoModule();
        ~LFOVibratoModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::TabbedComponent tabs {TabbedButtonBar::Orientation::TabsAtTop};
        LFOTab lfoTab {"FILTER LFO"}, vibratoTab {"VIBRATO"};
    
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOVibratoModule)
};
