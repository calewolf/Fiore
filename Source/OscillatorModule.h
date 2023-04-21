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
    
        // Radio buttons for choosing which waveforms to play
        juce::Label osc1ShapeMenuLabel, osc2ShapeMenuLabel;
        juce::ToggleButton sawButton1 {"Saw"}, squareButton1 {"Square"}, noiseButton1 {"Noise"};
        juce::ToggleButton sawButton2 {"Saw"}, squareButton2 {"Square"}, triButton2 {"Tri"};
    
        // Various oscillator parameters
        juce::Label sineLevelSliderLabel, vibratoSliderLabel, semitonesSliderLabel, centsSliderLabel;
        juce::Slider sineLevelSlider, vibratoSlider, semitonesSlider, centsSlider;
    
        // Mix of % OSC1 vs. % OSC2
        juce::Label oscMixSliderLabel1, oscMixSliderLabel2;
        juce::Slider oscMixSlider;
        
        // A big label that says "OSCILLATOR"
        juce::Label oscillatorModuleLabel;
    
        juce::AudioProcessorValueTreeState& apvts;

        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
        std::unique_ptr<SliderAttachment> osc1GainRatioAttachment;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorModule)
};
