/*
  ==============================================================================

    OscillatorModule.cpp
    Created: 9 Apr 2023 1:33:10pm
    Author:  Cale Wolf

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscillatorModule.h"

OscillatorModule::OscillatorModule() {
    // Oscillator shape combo boxes
    addAndMakeVisible(osc1ShapeMenuLabel);
    osc1ShapeMenuLabel.setText("OSC 1", juce::dontSendNotification);
    addAndMakeVisible(sawButton1);
    addAndMakeVisible(squareButton1);
    addAndMakeVisible(noiseButton1);
    addAndMakeVisible(osc2ShapeMenuLabel);
    osc2ShapeMenuLabel.setText("OSC 2", juce::dontSendNotification);
    addAndMakeVisible(sawButton2);
    addAndMakeVisible(squareButton2);
    addAndMakeVisible(triButton2);
    
    // Rotary sliders
    addAndMakeVisible(shapeModulationSlider);
    shapeModulationSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    shapeModulationSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    shapeModulationSlider.setRange(-1.0, 1.0);
    shapeModulationSlider.setValue(0.0);
    shapeModulationSlider.setNumDecimalPlacesToDisplay(2);
    
    addAndMakeVisible(shapeModulationSliderLabel);
    shapeModulationSliderLabel.setText("Shape Mod", juce::dontSendNotification);
    shapeModulationSliderLabel.setJustificationType(juce::Justification::centred);
    shapeModulationSliderLabel.attachToComponent(&shapeModulationSlider, false);
    
    addAndMakeVisible(vibratoSlider);
    vibratoSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    vibratoSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    vibratoSlider.setRange(0.0, 36.0);
    vibratoSlider.setTextValueSuffix(" st");
    vibratoSlider.setValue(0.0);
    vibratoSlider.setNumDecimalPlacesToDisplay(1);
    
    addAndMakeVisible(vibratoSliderLabel);
    vibratoSliderLabel.setText("Vibrato", juce::dontSendNotification);
    vibratoSliderLabel.setJustificationType(juce::Justification::centred);
    vibratoSliderLabel.attachToComponent(&vibratoSlider, false);
    
    addAndMakeVisible(semitonesSlider);
    semitonesSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    semitonesSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    semitonesSlider.setRange(-24.0, 24.0);
    semitonesSlider.setTextValueSuffix(" st");
    semitonesSlider.setValue(0.0);
    semitonesSlider.setNumDecimalPlacesToDisplay(0);
    
    addAndMakeVisible(semitonesSliderLabel);
    semitonesSliderLabel.setText("Semitones", juce::dontSendNotification);
    semitonesSliderLabel.setJustificationType(juce::Justification::centred);
    semitonesSliderLabel.attachToComponent(&semitonesSlider, false);
    
    addAndMakeVisible(centsSlider);
    centsSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    centsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    centsSlider.setRange(-50.0, 50.0);
    centsSlider.setTextValueSuffix(" c");
    centsSlider.setValue(0.0);
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
    
    addAndMakeVisible(oscMixSliderLabel1);
    oscMixSliderLabel1.setText("OSC1 Mix", juce::dontSendNotification);
    addAndMakeVisible(oscMixSliderLabel2);
    oscMixSliderLabel2.setText("OSC2 Mix", juce::dontSendNotification);
    
    // Big text label
    addAndMakeVisible(oscillatorModuleLabel);
    oscillatorModuleLabel.setText("OSCILLATOR", juce::dontSendNotification);
    oscillatorModuleLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    oscillatorModuleLabel.setJustificationType(juce::Justification::centred);
}

OscillatorModule::~OscillatorModule() {};

void OscillatorModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::lightsalmon);
}

void OscillatorModule::resized() {
    auto area = getLocalBounds();
    oscillatorModuleLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
    
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
    fb1.justifyContent = FlexBox::JustifyContent::spaceAround;
    fb1.alignItems = FlexBox::AlignItems::center;
    juce::FlexItem::Margin margin (24, 0, 0, 0);
    fb1.items = {
        juce::FlexItem(shapeModulationSlider).withWidth(90).withHeight(90).withMargin(margin),
        juce::FlexItem(semitonesSlider).withWidth(90).withHeight(90).withMargin(margin)
    };
    
    // Rotary knobs column 2
    juce::FlexBox fb2 (FlexBox::Direction::column, FlexBox::Wrap::noWrap, FlexBox::AlignContent::center, FlexBox::AlignItems::center, FlexBox::JustifyContent::spaceAround);
    fb2.items.add(juce::FlexItem(vibratoSlider).withWidth(90).withHeight(90).withMargin(margin));
    fb2.items.add(juce::FlexItem(centsSlider).withWidth(90).withHeight(90).withMargin(margin));
    
    // Osc mix slider
    juce::FlexBox fb3;
    fb3.flexDirection = FlexBox::Direction::column;
    fb3.justifyContent = FlexBox::JustifyContent::center;;
    fb3.alignItems = FlexBox::AlignItems::center;;
    fb3.items = {
        juce::FlexItem(oscMixSliderLabel1).withWidth(70).withHeight(24),
        juce::FlexItem(oscMixSlider).withWidth(30).withHeight(200),
        juce::FlexItem(oscMixSliderLabel2).withWidth(70).withHeight(24)
    };
    
    // Lay out into a parent flexbox!
    auto quarterWidth = area.getWidth() / 4;
    juce::FlexBox parentFlexBox;
    parentFlexBox.justifyContent = FlexBox::JustifyContent::center;
    parentFlexBox.flexDirection = FlexBox::Direction::row;
    parentFlexBox.items = {
        FlexItem(quarterWidth, area.getHeight(), fb0),
        FlexItem(quarterWidth, area.getHeight(), fb1),
        FlexItem(quarterWidth, area.getHeight(), fb2),
        FlexItem(quarterWidth, area.getHeight(), fb3)
    };
    parentFlexBox.performLayout(area.toFloat());
}

void OscillatorModule::comboBoxChanged(ComboBox *menu) {};

void OscillatorModule::sliderValueChanged(Slider *slider) {};
