#pragma once
#include <JuceHeader.h>

/**
    A single tab of the `LFOVibratoModule`. Used for both cutoff modulation and vibrato.
*/
class LFOTab: public juce::Component, Button::Listener {
    public:
        LFOTab(const String& title, juce::AudioProcessorValueTreeState& apvts, const String& paramIdPrefix);
        ~LFOTab();
        void paint (juce::Graphics&) override;
        void resized() override;
        void buttonClicked(Button* button) override;
    
    private:
        String paramIdPrefix;
    
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
    
        // Things to attach
        juce::AudioProcessorValueTreeState& apvts;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountSliderAttachment, rateSliderAttachment;
        
};

/**
    A module to allow users to control an LFO. Used for both filter cutoff modulation and vibrato.
*/
class LFOVibratoModule: public juce::Component {
    public:
        LFOVibratoModule(juce::AudioProcessorValueTreeState& apvts);
        ~LFOVibratoModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::TabbedComponent tabs {TabbedButtonBar::Orientation::TabsAtTop};
        LFOTab lfoTab, vibratoTab;
    
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOVibratoModule)
};
