#include <JuceHeader.h>
#include "AmpModule.h"

AmpModule::AmpModule() {
    // Big text label
    addAndMakeVisible(ampModuleLabel);
    ampModuleLabel.setText("AMP", juce::dontSendNotification);
    ampModuleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    ampModuleLabel.setJustificationType(juce::Justification::centred);
}

AmpModule::~AmpModule() {}

void AmpModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::lightgrey);
}

void AmpModule::resized() {
    auto area = getLocalBounds();
    ampModuleLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
}
