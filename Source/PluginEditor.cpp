#include "PluginProcessor.h"
#include "PluginEditor.h"


Editor::Editor (CapstoneSynthAudioProcessor& p): AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent (*p.getMidiKeyboardState(), MidiKeyboardComponent::horizontalKeyboard) {
    
    // Set up keyboard component
    addAndMakeVisible(keyboardComponent);
    
    // Waveform menus
    addAndMakeVisible(firstOscWaveformMenu);
    firstOscWaveformMenu.addItem("Sine", 1);
    firstOscWaveformMenu.addItem("Sawtooth", 2);
    firstOscWaveformMenu.addItem("Square", 3);
    firstOscWaveformMenu.setSelectedId(1);
    firstOscWaveformMenu.addListener(this);
    addAndMakeVisible(secondOscWaveformMenu);
    secondOscWaveformMenu.addItem("Sine", 1);
    secondOscWaveformMenu.addItem("Sawtooth", 2);
    secondOscWaveformMenu.addItem("Square", 3);
    secondOscWaveformMenu.setSelectedId(1);
    secondOscWaveformMenu.addListener(this);
    
    // Waveform menu labels
    addAndMakeVisible(firstOscLabel);
    addAndMakeVisible(secondOscLabel);
    firstOscLabel.setText("OSC 1: ", juce::dontSendNotification);
    secondOscLabel.setText("OSC 2: ", juce::dontSendNotification);
    firstOscLabel.attachToComponent(&firstOscWaveformMenu, true);
    secondOscLabel.attachToComponent(&secondOscWaveformMenu, true);
    
    // Oscillator Amp Sliders
    addAndMakeVisible(firstOscAmpSlider);
    addAndMakeVisible(secondOscAmpSlider);
    firstOscAmpSlider.setRange(0.0, 100.0);
    secondOscAmpSlider.setRange(0.0, 100.0);
    firstOscAmpSlider.setTextValueSuffix("%");
    secondOscAmpSlider.setTextValueSuffix("%");
    firstOscAmpSlider.addListener(this);
    secondOscAmpSlider.addListener(this);
    firstOscAmpSlider.setValue(firstOscAmp * 100);
    secondOscAmpSlider.setValue(secondOscAmp * 100);
    firstOscAmpSlider.setNumDecimalPlacesToDisplay(2);
    secondOscAmpSlider.setNumDecimalPlacesToDisplay(2);
    
    // Overall component size
    setSize (512, 184);
}

Editor::~Editor() {
}

void Editor::resized() {
    auto area = getLocalBounds();
    area.removeFromTop(8);
    area.removeFromLeft(8);
    area.removeFromRight(8);
    area.removeFromBottom(8);
    
    auto oscRow = area.removeFromTop(56);
    auto firstOscRow = oscRow.removeFromTop(24);
    firstOscWaveformMenu.setBounds(firstOscRow.removeFromLeft(128));
    firstOscRow.removeFromLeft(8);
    firstOscAmpSlider.setBounds(firstOscRow);
    oscRow.removeFromTop(8);
    auto secondOscRow = oscRow.removeFromTop(24);
    secondOscWaveformMenu.setBounds(secondOscRow.removeFromLeft(128));
    secondOscRow.removeFromLeft(8);
    secondOscAmpSlider.setBounds(secondOscRow);
    
    area.removeFromTop(8);
    keyboardComponent.setBounds(area);
}

void Editor::paint(juce::Graphics&) {
    
}

void Editor::comboBoxChanged(ComboBox *menu) {
    if (menu == &firstOscWaveformMenu) {
        firstOscWaveform = (Waveform) firstOscWaveformMenu.getSelectedId();
    } else if (menu == &secondOscWaveformMenu) {
        secondOscWaveform = (Waveform) secondOscWaveformMenu.getSelectedId();
    }
}

void Editor::sliderValueChanged(Slider* slider) {
    if (slider == &firstOscAmpSlider) {
        firstOscAmp = firstOscAmpSlider.getValue() / 100.0;
    } else if (slider == &secondOscAmpSlider) {
        secondOscAmp = secondOscAmpSlider.getValue() / 100.0;
    }
}

