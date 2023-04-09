#include <JuceHeader.h>
#include "FxModule.h"

FxModule::FxModule() {
    // Big text label
    addAndMakeVisible(fxModuleLabel);
    fxModuleLabel.setText("FX", juce::dontSendNotification);
    fxModuleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    fxModuleLabel.setJustificationType(juce::Justification::centred);
}

FxModule::~FxModule() {}

void FxModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::lightseagreen);
}

void FxModule::resized() {
    auto area = getLocalBounds();
    fxModuleLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
}
