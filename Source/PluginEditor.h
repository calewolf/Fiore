/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class Editor: public AudioProcessorEditor, ComboBox::Listener, Slider::Listener {
    
public:
    Editor(CapstoneSynthAudioProcessor&);
    ~Editor() override;
    
    // Component Overrides
    void paint(juce::Graphics&) override;
    void resized() override;
    
    // Listener Overrides
    void comboBoxChanged(ComboBox *menu) override;
    void sliderValueChanged(Slider* slider) override;

private:
    enum Waveform {
        Sine = 1,
        Sawtooth,
        Square
    };
    
    Label firstOscLabel;
    Label secondOscLabel;
    ComboBox firstOscWaveformMenu;
    ComboBox secondOscWaveformMenu;
    Slider firstOscAmpSlider;
    Slider secondOscAmpSlider;
    
    Waveform firstOscWaveform {Waveform::Sine};
    Waveform secondOscWaveform {Waveform::Sine};
    double firstOscAmp {0.75};
    double secondOscAmp {0.0};
    
    CapstoneSynthAudioProcessor& audioProcessor;
    MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
