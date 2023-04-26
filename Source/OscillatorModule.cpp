#include <JuceHeader.h>
#include "OscillatorModule.h"

void OscillatorModule::configureLabel(juce::Component& slider, juce::Label& label, const juce::String& labelText) {
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
}

void OscillatorModule::configureSlider(juce::Slider& slider, const juce::String textValueSuffix, int numDecimalPlacesToDisplay, const juce::String& paramID, std::unique_ptr<SliderAttachment>& attachmentToCreate) {
    addAndMakeVisible(slider);
    slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    slider.setTextValueSuffix(textValueSuffix);
    slider.setNumDecimalPlacesToDisplay(numDecimalPlacesToDisplay);
    slider.setDoubleClickReturnValue(true, apvts.getParameter(paramID)->getDefaultValue());
    attachmentToCreate = std::make_unique<SliderAttachment>(apvts, paramID, slider);
}

void OscillatorModule::configureRadioButton(juce::ToggleButton& button, RadioGroupID group, bool isOn) {
    addAndMakeVisible(button);
    button.setRadioGroupId(group);
    button.addListener(this);
    button.setToggleState(isOn, juce::dontSendNotification);
}

OscillatorModule::OscillatorModule(juce::AudioProcessorValueTreeState& apvts): apvts(apvts) {
    // Oscillator shape radio buttons (and labels)
    addAndMakeVisible(osc1ShapeMenuLabel);
    osc1ShapeMenuLabel.setText("OSC 1", juce::dontSendNotification);
    osc1ShapeMenuLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    addAndMakeVisible(osc2ShapeMenuLabel);
    osc2ShapeMenuLabel.setText("OSC 2", juce::dontSendNotification);
    osc2ShapeMenuLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    
    configureRadioButton(sawButton1, RadioGroupID::Osc1, true);
    configureRadioButton(squareButton1, RadioGroupID::Osc1);
    configureRadioButton(noiseButton1, RadioGroupID::Osc1);
    configureRadioButton(sawButton2, RadioGroupID::Osc2, true);
    configureRadioButton(squareButton2, RadioGroupID::Osc2);
    configureRadioButton(triButton2, RadioGroupID::Osc2);
    
    // Rotary sliders
    configureSlider(sineLevelSlider, "%", 0, "SINE_LVL", sineLevelAttachment);
    configureLabel(sineLevelSlider, sineLevelSliderLabel, "Sine Osc");
    
    configureSlider(osc1CentsSlider, " c", 1, "DETUNE_CENTS_1", osc1CentsAttachment);
    configureLabel(osc1CentsSlider, osc1CentsSliderLabel, "Cents (OSC1)");
    
    configureSlider(semitonesSlider, " st", 0, "DETUNE_SEMI", detuneSemiAttachment);
    configureLabel(semitonesSlider, semitonesSliderLabel, "Semi (OSC2)");
    
    configureSlider(centsSlider, " c", 0, "DETUNE_CENTS", detuneCentsAttachment);
    configureLabel(centsSlider, centsSliderLabel, "Cents (OSC2)");
    
    // Osc mix slider
    addAndMakeVisible(oscMixSlider);
    oscMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    oscMixSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    oscMixSlider.setDoubleClickReturnValue(true, 1.0);
    addAndMakeVisible(oscMixSliderLabel1);
    oscMixSliderLabel1.setText("OSC1 Mix", juce::dontSendNotification);
    addAndMakeVisible(oscMixSliderLabel2);
    oscMixSliderLabel2.setText("OSC2 Mix", juce::dontSendNotification);
    osc1GainRatioAttachment = std::make_unique<SliderAttachment>(apvts, "OSC1_GAIN_RATIO", oscMixSlider);
    
    // Big text label
    addAndMakeVisible(oscillatorModuleLabel);
    oscillatorModuleLabel.setText("OSCILLATOR", juce::dontSendNotification);
    oscillatorModuleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    oscillatorModuleLabel.setJustificationType(juce::Justification::centred);
}

OscillatorModule::~OscillatorModule() {};

void OscillatorModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::slategrey);
}

void OscillatorModule::resized() {
    auto area = getLocalBounds();
    oscillatorModuleLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
    
    auto lrMargin = area.getWidth() / 10;
    auto tbMargin = area.getHeight() / 10;
    area.removeFromTop(tbMargin);
    area.removeFromBottom(tbMargin);
    area.removeFromLeft(lrMargin);
    area.removeFromRight(lrMargin);
    
    // OSC selector radio buttons
    juce::FlexBox fb0;
    fb0.flexDirection = FlexBox::Direction::column;
    fb0.justifyContent = FlexBox::JustifyContent::center;
    fb0.alignItems = FlexBox::AlignItems::center;
    juce::FlexItem::Margin m {24, 0, 0, 0};
    fb0.items = {
        juce::FlexItem(osc1ShapeMenuLabel).withWidth(70).withHeight(24),
        juce::FlexItem(sawButton1).withWidth(70).withHeight(24),
        juce::FlexItem(squareButton1).withWidth(70).withHeight(24),
        juce::FlexItem(noiseButton1).withWidth(70).withHeight(24),
        juce::FlexItem(osc2ShapeMenuLabel).withWidth(70).withHeight(24).withMargin(m),
        juce::FlexItem(sawButton2).withWidth(70).withHeight(24),
        juce::FlexItem(squareButton2).withWidth(70).withHeight(24),
        juce::FlexItem(triButton2).withWidth(70).withHeight(24)
    };
    
    // Rotary knobs column 1
    juce::FlexBox fb1;
    fb1.flexDirection = FlexBox::Direction::column;
    fb1.justifyContent = FlexBox::JustifyContent::spaceBetween;
    fb1.alignItems = FlexBox::AlignItems::center;
    juce::FlexItem::Margin margin (24, 0, 0, 0);
    fb1.items = {
        juce::FlexItem(sineLevelSlider).withWidth(80).withHeight(80).withMargin(margin),
        juce::FlexItem(semitonesSlider).withWidth(80).withHeight(80).withMargin(margin)
    };
    
    // Rotary knobs column 2
    juce::FlexBox fb2;
    fb2.flexDirection = FlexBox::Direction::column;
    fb2.justifyContent = FlexBox::JustifyContent::spaceBetween;
    fb2.alignItems = FlexBox::AlignItems::center;
    fb2.items = {
        juce::FlexItem(osc1CentsSlider).withWidth(80).withHeight(80).withMargin(margin),
        juce::FlexItem(centsSlider).withWidth(80).withHeight(80).withMargin(margin)
    };
    
    // Osc mix slider
    juce::FlexBox fb3;
    fb3.flexDirection = FlexBox::Direction::column;
    fb3.justifyContent = FlexBox::JustifyContent::center;
    fb3.alignItems = FlexBox::AlignItems::center;
    fb3.alignContent = FlexBox::AlignContent::stretch;
    fb3.items = {
        juce::FlexItem(oscMixSliderLabel1).withWidth(70).withHeight(24),
        juce::FlexItem(oscMixSlider).withWidth(30).withMinHeight(100).withFlex(1),
        juce::FlexItem(oscMixSliderLabel2).withWidth(70).withHeight(24)
    };
    
    // Lay out into a parent flexbox!
    juce::FlexBox parentFlexBox;
    parentFlexBox.justifyContent = FlexBox::JustifyContent::spaceAround;
    parentFlexBox.items = {
        FlexItem(70, area.getHeight(), fb0),
        FlexItem(80, area.getHeight(), fb1),
        FlexItem(80, area.getHeight(), fb2),
        FlexItem(70, area.getHeight(), fb3)
    };
    parentFlexBox.performLayout(area.toFloat());
}

void OscillatorModule::buttonClicked(Button* button) {
    if (!button->getToggleState() == true) {
        return;
    }
    if (button == &sawButton1) {
        apvts.getParameterAsValue("OSC1_WAVE") = 0;
    } else if (button == &squareButton1) {
        apvts.getParameterAsValue("OSC1_WAVE") = 1;
    } else if (button == &noiseButton1) {
        apvts.getParameterAsValue("OSC1_WAVE") = 3;
    } else if (button == &sawButton2) {
        apvts.getParameterAsValue("OSC2_WAVE") = 0;
    } else if (button == &squareButton2) {
        apvts.getParameterAsValue("OSC2_WAVE") = 1;
    } else if (button == &triButton2) {
        apvts.getParameterAsValue("OSC2_WAVE") = 2;
    }
}
