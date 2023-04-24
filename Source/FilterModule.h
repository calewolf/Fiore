#pragma once

#include <JuceHeader.h>

/**
    A module for the synth's filter. Lets user select filter type, and adjust various parameters.
*/
class FilterModule: public juce::Component, public juce::Button::Listener {
    public:
        FilterModule(juce::AudioProcessorValueTreeState& apvts);
        ~FilterModule() override;
        void paint (juce::Graphics&) override;
        void resized() override;
        void buttonClicked (juce::Button* button) override;

    private:
        // Functions to clean up UI initialization
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
        void configureLabel(juce::Component& slider, juce::Label& label, const juce::String& labelText);
        void configureSlider(juce::Slider& slider, const juce::String textValueSuffix, int numDecimalPlacesToDisplay, const juce::String& paramID, std::unique_ptr<SliderAttachment>& attachmentToCreate);
    
        // UI components
        juce::TextButton onOffButton {"On"};
        juce::ComboBox filterTypeMenu;
        juce::Label cutoffSliderLabel, resonanceSliderLabel;
        juce::Slider cutoffSlider, resonanceSlider;
        juce::Label driveSliderLabel, envSliderLabel;
        juce::Slider driveSlider, envSlider;
        juce::Label filterModuleLabel;
    
        // Attachments to parameters
        std::unique_ptr<SliderAttachment> cutoffAttachment, resonanceAttachment, driveAttachment, envAttachment;
        std::unique_ptr<ComboBoxAttachment> filterTypeAttachment;
        juce::AudioProcessorValueTreeState& apvts;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterModule)
};
