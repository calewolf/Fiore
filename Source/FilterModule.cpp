#include <JuceHeader.h>
#include "FilterModule.h"

void FilterModule::configureSlider(juce::Slider& slider, const juce::String textValueSuffix, int numDecimalPlacesToDisplay, const juce::String& paramID, std::unique_ptr<SliderAttachment>& attachmentToCreate) {
    addAndMakeVisible(slider);
    slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    slider.setDoubleClickReturnValue(true, apvts.getParameter(paramID)->getDefaultValue());
    slider.setNumDecimalPlacesToDisplay(numDecimalPlacesToDisplay);
    slider.setTextValueSuffix(textValueSuffix);
    attachmentToCreate = std::make_unique<SliderAttachment>(apvts, paramID, slider);
}

void FilterModule::configureLabel(juce::Component& slider, juce::Label& label, const juce::String& labelText) {
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
}

FilterModule::FilterModule(juce::AudioProcessorValueTreeState& apvts): apvts(apvts) {
    // On-off button
    addAndMakeVisible(onOffButton);
    onOffButton.setToggleable(true);
    onOffButton.setClickingTogglesState(true);
    onOffButton.addListener(this);
    onOffButton.setToggleState(true, juce::sendNotification);
    
    // Filter type menu
    addAndMakeVisible(filterTypeMenu);
    juce::StringArray filterTypeChoices { "Lowpass (12dB/Oct)", "Highpass (12dB/Oct)", "Bandpass (12dB/Oct)", "Lowpass (24dB/Oct)", "Highpass (24dB/Oct)", "Bandpass (24dB/Oct)" };
    filterTypeMenu.addItemList(filterTypeChoices, 1);
    filterTypeMenu.setSelectedItemIndex(0);
    filterTypeAttachment = std::make_unique<ComboBoxAttachment>(apvts, "FILT_TYPE", filterTypeMenu);
    
    // Cutoff and resonance sliders
    addAndMakeVisible(cutoffSlider);
    cutoffSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 64, 24);
    cutoffSlider.setDoubleClickReturnValue(true, apvts.getParameter("FILT_CUTOFF")->getDefaultValue());
    cutoffSlider.setNumDecimalPlacesToDisplay(0);
    cutoffSlider.setTextValueSuffix("Hz");
    cutoffAttachment = std::make_unique<SliderAttachment>(apvts, "FILT_CUTOFF", cutoffSlider);
    addAndMakeVisible(cutoffSliderLabel);
    cutoffSliderLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffSliderLabel.attachToComponent(&cutoffSlider, false);
    
    configureSlider(resonanceSlider, "%", 0, "FILT_RESO", resonanceAttachment);
    configureLabel(resonanceSlider, resonanceSliderLabel, "Resonance");
    
    // LFO and Filter Envelope knobs
    configureSlider(driveSlider, "%", 0, "FILT_DRIVE_AMT", driveAttachment);
    configureLabel(driveSlider, driveSliderLabel, "Drive");
    configureSlider(envSlider, "%", 0, "FILT_ENV_AMT", envAttachment);
    configureLabel(envSlider, envSliderLabel, "Env");
    
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
        juce::FlexItem(driveSlider).withWidth(80).withHeight(80).withMargin(m2),
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
    driveSlider.setEnabled(isOn);
    envSlider.setEnabled(isOn);
    
    apvts.getParameterAsValue("FILT_ON_OFF") = isOn;
}
