#include <JuceHeader.h>
#include "EnvModule.h"

EnvTab::EnvTab(const String& title) {
    // Title label
    addAndMakeVisible(titleLabel);
    titleLabel.setText(title, juce::dontSendNotification);
    titleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    
    // Attack
    addAndMakeVisible(attackSlider);
    attackSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 24);
    attackSlider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(attackSliderLabel);
    attackSliderLabel.setText("Atk", juce::dontSendNotification);
    attackSliderLabel.setJustificationType(juce::Justification::centred);
    attackSliderLabel.attachToComponent(&attackSlider, false);
    
    // Decay
    addAndMakeVisible(decaySlider);
    decaySlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 24);
    decaySlider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(decaySliderLabel);
    decaySliderLabel.setText("Dec", juce::dontSendNotification);
    decaySliderLabel.setJustificationType(juce::Justification::centred);
    decaySliderLabel.attachToComponent(&decaySlider, false);
    
    // Sustain
    addAndMakeVisible(sustainSlider);
    sustainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 24);
    sustainSlider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(sustainSliderLabel);
    sustainSliderLabel.setText("Sus", juce::dontSendNotification);
    sustainSliderLabel.setJustificationType(juce::Justification::centred);
    sustainSliderLabel.attachToComponent(&sustainSlider, false);
    
    // Release
    addAndMakeVisible(releaseSlider);
    releaseSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 24);
    releaseSlider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(releaseSliderLabel);
    releaseSliderLabel.setText("Rel", juce::dontSendNotification);
    releaseSliderLabel.setJustificationType(juce::Justification::centred);
    releaseSliderLabel.attachToComponent(&releaseSlider, false);
}

EnvTab::~EnvTab() {
    
}

void EnvTab::paint (juce::Graphics&) {
    
}

void EnvTab::resized() {
    auto area = getLocalBounds();
    titleLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
    
    area.removeFromLeft(area.getWidth() * 0.1);
    area.removeFromRight(area.getWidth() * 0.1);
    
    juce::FlexBox fb;
    fb.justifyContent = FlexBox::JustifyContent::spaceAround;
    fb.alignItems = FlexBox::AlignItems::center;
    juce::FlexItem::Margin m {24, 0, 0, 0};
    fb.items = {
        juce::FlexItem(attackSlider).withWidth(50).withHeight(150).withMargin(m),
        juce::FlexItem(decaySlider).withWidth(50).withHeight(150).withMargin(m),
        juce::FlexItem(sustainSlider).withWidth(50).withHeight(150).withMargin(m),
        juce::FlexItem(releaseSlider).withWidth(50).withHeight(150).withMargin(m)
    };
    
    fb.performLayout(area.toFloat());
}

// EnvModule stuff

EnvModule::EnvModule() {
    addAndMakeVisible(tabs);
    tabs.addTab("Amplitude Env", juce::Colours::slategrey, &ampEnvTab, false);
    tabs.addTab("Filter Env", juce::Colours::slategrey.darker(), &filterEnvTab, false);
}

EnvModule::~EnvModule() {
}

void EnvModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::slategrey.darker().darker());
}

void EnvModule::resized() {
    tabs.setBounds(getLocalBounds());
}
