#include <JuceHeader.h>
#include "EnvModule.h"

EnvTab::EnvTab(const String& title, juce::AudioProcessorValueTreeState& apvts, const String& paramIdPrefix): apvts(apvts) {
    // Title label
    addAndMakeVisible(titleLabel);
    titleLabel.setText(title, juce::dontSendNotification);
    titleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    
    // Attack
    configureSlider(attackSlider, "s", 1, paramIdPrefix + "_ATK", attackAttachment);
    configureLabel(attackSlider, attackSliderLabel, "Atk");
    
    // Decay
    configureSlider(decaySlider, "s", 1, paramIdPrefix + "_DEC", decayAttachment);
    configureLabel(decaySlider, decaySliderLabel, "Dec");
    
    // Sustain
    configureSlider(sustainSlider, "%", 0, paramIdPrefix + "_SUS", sustainAttachment);
    configureLabel(sustainSlider, sustainSliderLabel, "Sus");
    
    // Release
    configureSlider(releaseSlider, "s", 1, paramIdPrefix + "_REL", releaseAttachment);
    configureLabel(releaseSlider, releaseSliderLabel, "Rel");
}

void EnvTab::configureSlider(juce::Slider& slider, const juce::String textValueSuffix, int numDecimalPlacesToDisplay, const juce::String& paramID, std::unique_ptr<SliderAttachment>& attachmentToCreate) {
    addAndMakeVisible(slider);
    slider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 24);
    slider.setTextValueSuffix(textValueSuffix);
    slider.setNumDecimalPlacesToDisplay(numDecimalPlacesToDisplay);
    slider.setDoubleClickReturnValue(true, apvts.getParameter(paramID)->getDefaultValue());
    attachmentToCreate = std::make_unique<SliderAttachment>(apvts, paramID, slider);
}

void EnvTab::configureLabel(juce::Component& slider, juce::Label& label, const juce::String& labelText) {
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
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

EnvModule::EnvModule(juce::AudioProcessorValueTreeState& apvts): ampEnvTab("AMP ENV", apvts, "AMP"), filterEnvTab("FILTER ENV", apvts, "FILT") {
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
