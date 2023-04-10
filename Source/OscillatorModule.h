/*
  ==============================================================================

    OscillatorModule.h
    Created: 9 Apr 2023 1:33:10pm
    Author:  Cale Wolf

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscillatorModule: public juce::Component, ComboBox::Listener, Slider::Listener {
    public:
        OscillatorModule();
        ~OscillatorModule() override;
        void paint(juce::Graphics&) override;
        void resized() override;
        void comboBoxChanged(ComboBox *menu) override;
        void sliderValueChanged(Slider* slider) override;
    
    private:
        enum Waveform {
            Sine = 1,
            Sawtooth,
            Square
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
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorModule)
};
