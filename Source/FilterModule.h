#pragma once

#include <JuceHeader.h>

class FilterModule: public juce::Component {
    public:
        FilterModule();
        ~FilterModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;

    private:
        juce::TextButton onOffButton {"On"};
        juce::ComboBox filterTypeMenu;
        
        // TODO: Insert filter GUI box
        juce::Label cutoffSliderLabel, resonanceSliderLabel;
        juce::Slider cutoffSlider, resonanceSlider;
        
        juce::Label lfoSliderLabel, envSliderLabel;
        juce::Slider lfoSlider, envSlider;
        
        juce::Label filterModuleLabel;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterModule)
};
