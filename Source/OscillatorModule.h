#pragma once

#include <JuceHeader.h>

class OscillatorModule: public juce::Component, Button::Listener {
    public:
        OscillatorModule(juce::AudioProcessorValueTreeState& apvts);
        ~OscillatorModule() override;
        void paint(juce::Graphics&) override;
        void resized() override;
        void buttonClicked(Button* button) override;
    
    private:
        enum RadioGroupID {
            Osc1 = 1001,
            Osc2 = 1002
        };
    
        // Functions for cleaning up initialization code
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        void configureRadioButton(juce::ToggleButton& button, RadioGroupID group, bool isOn = false);
        void configureLabel(juce::Component& slider, juce::Label& label, const juce::String& labelText);
        void configureSlider(juce::Slider& slider, const juce::String textValueSuffix, int numDecimalPlacesToDisplay, const juce::String& paramID, std::unique_ptr<SliderAttachment>& attachmentToCreate);
    
        // Radio buttons for choosing which waveforms to play
        juce::Label osc1ShapeMenuLabel, osc2ShapeMenuLabel;
        juce::ToggleButton sawButton1 {"Saw"}, squareButton1 {"Square"}, noiseButton1 {"Noise"};
        juce::ToggleButton sawButton2 {"Saw"}, squareButton2 {"Square"}, triButton2 {"Tri"};
    
        // Various oscillator parameters
        juce::Label sineLevelSliderLabel, mysterySliderLabel, semitonesSliderLabel, centsSliderLabel;
        juce::Slider sineLevelSlider, mysterySlider, semitonesSlider, centsSlider;
    
        // Mix of % OSC1 vs. % OSC2
        juce::Label oscMixSliderLabel1, oscMixSliderLabel2;
        juce::Slider oscMixSlider;
        
        // A big label that says "OSCILLATOR"
        juce::Label oscillatorModuleLabel;
    
        // Stuff for attaching sliders to parameters
        juce::AudioProcessorValueTreeState& apvts;
        std::unique_ptr<SliderAttachment> osc1GainRatioAttachment, detuneCentsAttachment, detuneSemiAttachment, mysteryAttachment, sineLevelAttachment;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorModule)
};
