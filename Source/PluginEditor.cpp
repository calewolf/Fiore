#include "PluginProcessor.h"
#include "PluginEditor.h"

Editor::Editor (CapstoneSynthAudioProcessor& p): AudioProcessorEditor (&p), audioProcessor (p) {
    addAndMakeVisible(oscModule);
    addAndMakeVisible(filterModule);
    addAndMakeVisible(ampModule);
    addAndMakeVisible(fxModule);
    addAndMakeVisible(bendModule);
    addAndMakeVisible(lfoVibratoModule);
    addAndMakeVisible(filterEnvModule);
    addAndMakeVisible(ampEnvModule);
    
    setSize (1050, 600);
}

Editor::~Editor() {
}

void Editor::resized() {
    auto area = getLocalBounds();
    
    auto topRow = area.removeFromTop(300);
    auto bottomRow = area;
    
    oscModule.setBounds(topRow.removeFromLeft(450));
    filterModule.setBounds(topRow.removeFromLeft(300));
    ampModule.setBounds(topRow.removeFromLeft(150));
    fxModule.setBounds(topRow.removeFromLeft(150));
    
    bendModule.setBounds(bottomRow.removeFromLeft(150));
    lfoVibratoModule.setBounds(bottomRow.removeFromLeft(300));
    filterEnvModule.setBounds(bottomRow.removeFromLeft(300));
    ampEnvModule.setBounds(bottomRow.removeFromLeft(300));
}

void Editor::paint(juce::Graphics&) {
    
}
