#include <JuceHeader.h>
#include "FilterModule.h"

FilterModule::FilterModule() {
    
    addAndMakeVisible(onOffButton);
    onOffButton.setToggleable(true);
    onOffButton.setClickingTogglesState(true);
    
    addAndMakeVisible(filterTypeMenu);
    
    // Cutoff and resonance sliders
    addAndMakeVisible(cutoffSlider);
    cutoffSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 64, 24);
    cutoffSlider.setRange(0.0, 1.0);
    cutoffSlider.setValue(0.5);
    cutoffSlider.setNumDecimalPlacesToDisplay(2);
    
    addAndMakeVisible(cutoffSliderLabel);
    cutoffSliderLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffSliderLabel.attachToComponent(&cutoffSlider, false);
    
    addAndMakeVisible(resonanceSlider);
    resonanceSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    resonanceSlider.setRange(0.0, 1.0);
    resonanceSlider.setValue(0.5);
    resonanceSlider.setNumDecimalPlacesToDisplay(2);
    
    addAndMakeVisible(resonanceSliderLabel);
    resonanceSliderLabel.setText("Resonance", juce::dontSendNotification);
    resonanceSliderLabel.attachToComponent(&resonanceSlider, false);
    
    // LFO and Filter Envelope knobs
    addAndMakeVisible(lfoSlider);
    lfoSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    lfoSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    lfoSlider.setRange(0.0, 1.0);
    lfoSlider.setValue(0.0);
    lfoSlider.setNumDecimalPlacesToDisplay(2);
    
    addAndMakeVisible(lfoSliderLabel);
    lfoSliderLabel.setText("LFO", juce::dontSendNotification);
    lfoSliderLabel.setJustificationType(juce::Justification::centred);
    lfoSliderLabel.attachToComponent(&lfoSlider, false);
    
    addAndMakeVisible(envSlider);
    envSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    envSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    envSlider.setRange(0.0, 1.0);
    envSlider.setValue(0.0);
    envSlider.setNumDecimalPlacesToDisplay(2);
    
    addAndMakeVisible(envSliderLabel);
    envSliderLabel.setText("Env", juce::dontSendNotification);
    envSliderLabel.setJustificationType(juce::Justification::centred);
    envSliderLabel.attachToComponent(&envSlider, false);
    
    // Big text label
    addAndMakeVisible(filterModuleLabel);
    filterModuleLabel.setText("FILTER", juce::dontSendNotification);
    filterModuleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    filterModuleLabel.setJustificationType(juce::Justification::centred);
}

FilterModule::~FilterModule() {}

void FilterModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::slategrey.darker());
}

void FilterModule::resized() {
    auto area = getLocalBounds();
    filterModuleLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
    
    area.removeFromTop(area.getHeight() / 10);
    area.removeFromBottom(area.getHeight() / 10);
    
    juce::FlexBox row1;
    row1.justifyContent = FlexBox::JustifyContent::center;
    row1.alignItems = FlexBox::AlignItems::center;
    juce::FlexItem::Margin m3 (0, 8, 0, 0);
    row1.items = {
        juce::FlexItem(onOffButton).withWidth(48).withHeight(24).withMargin(m3),
        juce::FlexItem(filterTypeMenu).withWidth(194).withHeight(24)
    };
    
    juce::FlexBox row2;
    row2.flexDirection = FlexBox::Direction::column;
    row2.justifyContent = FlexBox::JustifyContent::center;
    row2.alignItems = FlexBox::AlignItems::center;
    juce::FlexItem::Margin margin (24, 0, 0, 0);
    row2.items = {
        juce::FlexItem(cutoffSlider).withWidth(240).withHeight(24).withMargin(margin),
        
    };
    
    juce::FlexBox row3;
    row3.justifyContent = FlexBox::JustifyContent::center;
    row3.alignItems = FlexBox::AlignItems::center;;
    row3.items = {
        juce::FlexItem(resonanceSlider).withWidth(80).withHeight(80).withMargin(margin),
        juce::FlexItem(lfoSlider).withWidth(80).withHeight(80).withMargin(margin),
        juce::FlexItem(envSlider).withWidth(80).withHeight(80).withMargin(margin)
    };
    
    
    juce::FlexBox parentFlexBox;
    parentFlexBox.flexDirection = FlexBox::Direction::column;
    parentFlexBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    parentFlexBox.items = {
        FlexItem(area.getWidth(), 24, row1),
        FlexItem(area.getWidth(), 48, row2),
        FlexItem(area.getWidth(), 104, row3)
    };
    parentFlexBox.performLayout(area.toFloat());
}
