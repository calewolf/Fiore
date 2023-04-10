#include <JuceHeader.h>
#include "AmpModule.h"

AmpModule::AmpModule() {
    // Volume slider
    addAndMakeVisible(volumeSlider);
    volumeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    volumeSlider.setRange(-1.0, 1.0);
    volumeSlider.setValue(0.0);
    volumeSlider.setNumDecimalPlacesToDisplay(2);
    
    addAndMakeVisible(volumeSliderLabel);
    volumeSliderLabel.setText("Volume", juce::dontSendNotification);
    volumeSliderLabel.setJustificationType(juce::Justification::centred);
    volumeSliderLabel.attachToComponent(&volumeSlider, false);
    
    // Big text label
    addAndMakeVisible(ampModuleLabel);
    ampModuleLabel.setText("AMP", juce::dontSendNotification);
    ampModuleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    ampModuleLabel.setJustificationType(juce::Justification::centred);
}

AmpModule::~AmpModule() {}

void AmpModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::slategrey.brighter());
}

void AmpModule::resized() {
    auto area = getLocalBounds();
    ampModuleLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
    
    juce::FlexBox parentFlexBox;
    parentFlexBox.flexDirection = FlexBox::Direction::column;
    parentFlexBox.justifyContent = FlexBox::JustifyContent::spaceAround;;
    parentFlexBox.alignItems = FlexBox::AlignItems::center;;
    juce::FlexItem::Margin margin (24, 0, 0, 0);
    parentFlexBox.items = {
        FlexItem(volumeSlider).withWidth(90).withHeight(90).withMargin(margin)
    };
    parentFlexBox.performLayout(area.toFloat());
}
