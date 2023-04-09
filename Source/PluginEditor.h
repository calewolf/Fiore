/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


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
        juce::ToggleButton sawButton2 {"Saw"}, squareButton2 {"Square"}, triButton2 {"Triangle"};
    
        // Various oscillator parameters
        juce::Label shapeModulationSliderLabel, vibratoSliderLabel, semitonesSliderLabel, centsSliderLabel;
        juce::Slider shapeModulationSlider, vibratoSlider, semitonesSlider, centsSlider;
    
        // Mix of % OSC1 vs. % OSC2
        juce::Label oscMixSliderLabel1, oscMixSliderLabel2;
        juce::Slider oscMixSlider;
        
        // A big label that says "OSCILLATOR"
        juce::Label oscillatorModuleLabel;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorModule)
};


class Editor: public AudioProcessorEditor {
    
public:
    Editor(CapstoneSynthAudioProcessor&);
    ~Editor() override;
    
    // Component Overrides
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OscillatorModule oscModule;
    CapstoneSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
