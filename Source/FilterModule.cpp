#include <JuceHeader.h>
#include "FilterModule.h"

FilterModule::FilterModule() {
    // Big text label
    addAndMakeVisible(filterModuleLabel);
    filterModuleLabel.setText("FILTER", juce::dontSendNotification);
    filterModuleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    filterModuleLabel.setJustificationType(juce::Justification::centred);
}

FilterModule::~FilterModule() {}

void FilterModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::lightpink);
}

void FilterModule::resized() {
    auto area = getLocalBounds();
    filterModuleLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
}
