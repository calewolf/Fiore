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
        juce::Slider keySlider;
        juce::Label keySliderLabel;
        
        juce::Label filterFMSliderLabel, lfoSliderLabel, envSliderLabel;
        juce::Slider filterFMSlider, lfoSlider, envSlider;
        
        juce::Label filterModuleLabel;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterModule)
};
