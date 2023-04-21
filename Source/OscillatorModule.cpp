#include <JuceHeader.h>
#include "OscillatorModule.h"

OscillatorModule::OscillatorModule(juce::AudioProcessorValueTreeState& apvts): apvts(apvts) {
    // Oscillator shape combo boxes
    addAndMakeVisible(osc1ShapeMenuLabel);
    osc1ShapeMenuLabel.setText("OSC 1", juce::dontSendNotification);
    osc1ShapeMenuLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    addAndMakeVisible(sawButton1);
    sawButton1.setRadioGroupId(RadioGroupID::Osc1);
    sawButton1.setToggleState(true, juce::dontSendNotification);
    sawButton1.addListener(this);
    addAndMakeVisible(squareButton1);
    squareButton1.setRadioGroupId(RadioGroupID::Osc1);
    squareButton1.addListener(this);
    addAndMakeVisible(noiseButton1);
    noiseButton1.setRadioGroupId(RadioGroupID::Osc1);
    noiseButton1.addListener(this);
    
    addAndMakeVisible(osc2ShapeMenuLabel);
    osc2ShapeMenuLabel.setText("OSC 2", juce::dontSendNotification);
    osc2ShapeMenuLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    addAndMakeVisible(sawButton2);
    sawButton2.addListener(this);
    sawButton2.setRadioGroupId(RadioGroupID::Osc2);
    sawButton2.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(squareButton2);
    squareButton2.addListener(this);
    squareButton2.setRadioGroupId(RadioGroupID::Osc2);
    addAndMakeVisible(triButton2);
    triButton2.addListener(this);
    triButton2.setRadioGroupId(RadioGroupID::Osc2);
    
    // Rotary sliders
    addAndMakeVisible(sineLevelSlider);
    sineLevelSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    sineLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    sineLevelSlider.setRange(0, 100);
    sineLevelSlider.setDoubleClickReturnValue(true, 0);
    sineLevelSlider.setTextValueSuffix("%");
    sineLevelSlider.setValue(0);
    sineLevelSlider.setNumDecimalPlacesToDisplay(0);
    
    addAndMakeVisible(sineLevelSliderLabel);
    sineLevelSliderLabel.setText("Sine Level", juce::dontSendNotification);
    sineLevelSliderLabel.setJustificationType(juce::Justification::centred);
    sineLevelSliderLabel.attachToComponent(&sineLevelSlider, false);
    
    addAndMakeVisible(vibratoSlider);
    vibratoSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    vibratoSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    vibratoSlider.setRange(0.0, 36.0);
    vibratoSlider.setTextValueSuffix(" st");
    vibratoSlider.setValue(0.0);
    vibratoSlider.setDoubleClickReturnValue(true, 0.0);
    vibratoSlider.setNumDecimalPlacesToDisplay(1);
    
    addAndMakeVisible(vibratoSliderLabel);
    vibratoSliderLabel.setText("Vibrato", juce::dontSendNotification);
    vibratoSliderLabel.setJustificationType(juce::Justification::centred);
    vibratoSliderLabel.attachToComponent(&vibratoSlider, false);
    
    addAndMakeVisible(semitonesSlider);
    semitonesSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    semitonesSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    semitonesSlider.setRange(-24.0, 24.0);
    semitonesSlider.setTextValueSuffix(" st");
    semitonesSlider.setValue(0.0);
    semitonesSlider.setDoubleClickReturnValue(true, 0);
    semitonesSlider.setNumDecimalPlacesToDisplay(0);
    
    addAndMakeVisible(semitonesSliderLabel);
    semitonesSliderLabel.setText("Semitones", juce::dontSendNotification);
    semitonesSliderLabel.setJustificationType(juce::Justification::centred);
    semitonesSliderLabel.attachToComponent(&semitonesSlider, false);
    
    addAndMakeVisible(centsSlider);
    centsSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    centsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    centsSlider.setRange(-50.0, 50.0);
    centsSlider.setTextValueSuffix(" c");
    centsSlider.setValue(0.0);
    centsSlider.setDoubleClickReturnValue(true, 0);
    centsSlider.setNumDecimalPlacesToDisplay(0);
    
    addAndMakeVisible(centsSliderLabel);
    centsSliderLabel.setText("Cents", juce::dontSendNotification);
    centsSliderLabel.setJustificationType(juce::Justification::centred);
    centsSliderLabel.attachToComponent(&centsSlider, false);
    
    // Osc mix slider
    addAndMakeVisible(oscMixSlider);
    oscMixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    oscMixSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    oscMixSlider.setRange(0, 100);
    oscMixSlider.setValue(50.0);
    oscMixSlider.setDoubleClickReturnValue(true, 50);
    
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
        juce::FlexItem(vibratoSlider).withWidth(80).withHeight(80).withMargin(margin),
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
    if (button == &sawButton1) {
        apvts.getParameterAsValue("OSC1_WAVE") = 0;
    } else if (button == &squareButton1) {
        apvts.getParameterAsValue("OSC1_WAVE") = 1;
    } else if (button == &noiseButton1) {
        apvts.getParameterAsValue("OSC1_WAVE") = 2;
    } else if (button == &sawButton2) {
        apvts.getParameterAsValue("OSC2_WAVE") = 0;
    } else if (button == &squareButton2) {
        apvts.getParameterAsValue("OSC2_WAVE") = 1;
    } else if (button == &triButton2) {
        apvts.getParameterAsValue("OSC2_WAVE") = 2;
    }
}
