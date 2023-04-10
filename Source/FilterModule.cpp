#include <JuceHeader.h>
#include "FilterModule.h"

FilterModule::FilterModule() {
    addAndMakeVisible(onOffButton);
    onOffButton.setToggleable(true);
    onOffButton.setClickingTogglesState(true);
    onOffButton.addListener(this);
    onOffButton.setToggleState(true, juce::sendNotification);
    
    addAndMakeVisible(filterTypeMenu);
    filterTypeMenu.setTextWhenNothingSelected("Select filter:");
    filterTypeMenu.addItem("Lowpass", 1);
    filterTypeMenu.addItem("Highpass", 2);
    filterTypeMenu.addItem("Bandpass", 3);
    filterTypeMenu.setSelectedId(1);
    
    // Cutoff and resonance sliders
    addAndMakeVisible(cutoffSlider);
    cutoffSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 64, 24);
    cutoffSlider.setRange(20.0, 10000.0);
    cutoffSlider.setSkewFactorFromMidPoint(500.0);
    cutoffSlider.setValue(500.0);
    cutoffSlider.setDoubleClickReturnValue(true, 500.0);
    cutoffSlider.setNumDecimalPlacesToDisplay(0);
    cutoffSlider.setTextValueSuffix("Hz");
    
    addAndMakeVisible(cutoffSliderLabel);
    cutoffSliderLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffSliderLabel.attachToComponent(&cutoffSlider, false);
    
    addAndMakeVisible(resonanceSlider);
    resonanceSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    resonanceSlider.setRange(0, 100.0);
    resonanceSlider.setSkewFactorFromMidPoint(50.0);
    resonanceSlider.setValue(0);
    resonanceSlider.setDoubleClickReturnValue(true, 0);
    resonanceSlider.setNumDecimalPlacesToDisplay(0);
    resonanceSlider.setTextValueSuffix("%");
    
    addAndMakeVisible(resonanceSliderLabel);
    resonanceSliderLabel.setText("Resonance", juce::dontSendNotification);
    resonanceSliderLabel.attachToComponent(&resonanceSlider, false);
    
    // LFO and Filter Envelope knobs
    addAndMakeVisible(lfoSlider);
    lfoSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    lfoSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    lfoSlider.setRange(0, 100);
    lfoSlider.setValue(0.0);
    lfoSlider.setDoubleClickReturnValue(true, 0.0);
    lfoSlider.setNumDecimalPlacesToDisplay(0);
    lfoSlider.setTextValueSuffix("%");
    
    addAndMakeVisible(lfoSliderLabel);
    lfoSliderLabel.setText("LFO", juce::dontSendNotification);
    lfoSliderLabel.setJustificationType(juce::Justification::centred);
    lfoSliderLabel.attachToComponent(&lfoSlider, false);
    
    addAndMakeVisible(envSlider);
    envSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    envSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    envSlider.setRange(-100, 100);
    envSlider.setSkewFactorFromMidPoint(0.0);
    envSlider.setValue(0.0);
    envSlider.setDoubleClickReturnValue(true, 0.0);
    envSlider.setNumDecimalPlacesToDisplay(0);
    envSlider.setTextValueSuffix("%");
    
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
    
    auto lrMargin = area.getWidth() / 10;
    auto tbMargin = area.getWidth() / 10;
    area.removeFromTop(tbMargin);
    area.removeFromBottom(tbMargin);
    area.removeFromLeft(lrMargin);
    area.removeFromRight(lrMargin);
    
    juce::FlexBox row1;
    row1.justifyContent = FlexBox::JustifyContent::flexStart;
    row1.alignItems = FlexBox::AlignItems::stretch;
    juce::FlexItem::Margin m3 (0, 8, 0, 0);
    row1.items = {
        juce::FlexItem(onOffButton).withWidth(48).withHeight(24).withMargin(m3),
        juce::FlexItem(filterTypeMenu).withWidth(194).withHeight(24).withFlex(1)
    };
    
    juce::FlexBox row2;
    row2.flexDirection = FlexBox::Direction::column;
    row2.justifyContent = FlexBox::JustifyContent::center;
    row2.alignItems = FlexBox::AlignItems::stretch;
    juce::FlexItem::Margin m1 (24, 8, 0, 0);
    row2.items = {
        juce::FlexItem(cutoffSlider).withMinWidth(240).withHeight(24).withMargin(m1).withFlex(1),
    };
    
    juce::FlexBox row3;
    row3.justifyContent = FlexBox::JustifyContent::spaceBetween;
    row3.alignItems = FlexBox::AlignItems::center;;
    juce::FlexItem::Margin m2 (24, 0, 0, 0);
    row3.items = {
        juce::FlexItem(resonanceSlider).withWidth(80).withHeight(80).withMargin(m2),
        juce::FlexItem(lfoSlider).withWidth(80).withHeight(80).withMargin(m2),
        juce::FlexItem(envSlider).withWidth(80).withHeight(80).withMargin(m2)
    };
    
    
    juce::FlexBox parentFlexBox;
    parentFlexBox.flexDirection = FlexBox::Direction::column;
    parentFlexBox.justifyContent = FlexBox::JustifyContent::spaceBetween;
    parentFlexBox.items = {
        FlexItem(area.getWidth(), 24, row1),
        FlexItem(area.getWidth(), 48, row2),
        FlexItem(area.getWidth(), 104, row3)
    };
    parentFlexBox.performLayout(area.toFloat());
}

void FilterModule::buttonClicked(juce::Button* button) {
    auto isOn = button->getToggleState();
    button->setButtonText(isOn ? "On" : "Off");
    
    filterTypeMenu.setEnabled(isOn);
    cutoffSlider.setEnabled(isOn);
    resonanceSlider.setEnabled(isOn);
    lfoSlider.setEnabled(isOn);
    envSlider.setEnabled(isOn);
}
