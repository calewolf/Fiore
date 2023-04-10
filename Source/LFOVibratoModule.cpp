#include <JuceHeader.h>
#include "LFOVibratoModule.h"

// LFO Tab //

LFOTab::LFOTab(const String& title) {
    // Shape options
    addAndMakeVisible(lfoShapeLabel);
    lfoShapeLabel.setText("Shape", juce::dontSendNotification);
    lfoShapeLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    addAndMakeVisible(sawUpButton);
    addAndMakeVisible(sawDownButton);
    addAndMakeVisible(triangleButton);
    addAndMakeVisible(squareButton);
    addAndMakeVisible(noiseButton);
    
    // Amount slider
    addAndMakeVisible(lfoAmountSlider);
    lfoAmountSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    lfoAmountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    lfoAmountSlider.setRange(0, 100);
    lfoAmountSlider.setValue(25);
    lfoAmountSlider.setDoubleClickReturnValue(true, 25);
    lfoAmountSlider.setNumDecimalPlacesToDisplay(0);
    lfoAmountSlider.setTextValueSuffix("%");
    
    addAndMakeVisible(lfoAmountSliderLabel);
    lfoAmountSliderLabel.setText("Amp", juce::dontSendNotification);
    lfoAmountSliderLabel.setJustificationType(juce::Justification::centred);
    lfoAmountSliderLabel.attachToComponent(&lfoAmountSlider, false);
    
    // Rate slider
    addAndMakeVisible(lfoRateSlider);
    lfoRateSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    lfoRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 24);
    lfoRateSlider.setRange(0.01, 200);
    lfoRateSlider.setValue(1.0);
    lfoRateSlider.setSkewFactorFromMidPoint(15);
    lfoRateSlider.setDoubleClickReturnValue(true, 1.0);
    lfoRateSlider.setNumDecimalPlacesToDisplay(2);
    lfoRateSlider.setTextValueSuffix("Hz");
    
    addAndMakeVisible(lfoRateSliderLabel);
    lfoRateSliderLabel.setText("Rate", juce::dontSendNotification);
    lfoRateSliderLabel.setJustificationType(juce::Justification::centred);
    lfoRateSliderLabel.attachToComponent(&lfoRateSlider, false);

    // Big text label
    addAndMakeVisible(lfoTabLabel);
    lfoTabLabel.setText(title, juce::dontSendNotification);
    lfoTabLabel.setFont(juce::Font (16.0f, juce::Font::bold));
    lfoTabLabel.setJustificationType(juce::Justification::centred);
}

LFOTab::~LFOTab() {}

void LFOTab::paint (juce::Graphics&) {}

void LFOTab::resized() {
    auto area = getLocalBounds();
    lfoTabLabel.setBounds(area.removeFromBottom(area.getHeight() / 10));
    
    juce::FlexBox col1;
    col1.flexDirection = FlexBox::Direction::column;
    col1.justifyContent = FlexBox::JustifyContent::center;
    col1.alignItems = FlexBox::AlignItems::center;
    col1.items = {
        juce::FlexItem(lfoShapeLabel).withWidth(100).withHeight(24),
        juce::FlexItem(sawUpButton).withWidth(100).withHeight(24),
        juce::FlexItem(sawDownButton).withWidth(100).withHeight(24),
        juce::FlexItem(triangleButton).withWidth(100).withHeight(24),
        juce::FlexItem(squareButton).withWidth(100).withHeight(24),
        juce::FlexItem(noiseButton).withWidth(100).withHeight(24)
    };
    
    juce::FlexBox col2;
    col2.justifyContent = FlexBox::JustifyContent::spaceAround;
    col2.alignItems = FlexBox::AlignItems::center;
    juce::FlexItem::Margin margin (24, 0, 0, 0);
    col2.items = {
        juce::FlexItem(lfoAmountSlider).withWidth(50).withHeight(150).withMargin(margin),
        juce::FlexItem(lfoRateSlider).withWidth(50).withHeight(150).withMargin(margin),
    };
    
    juce::FlexBox fb;
    fb.alignItems = FlexBox::AlignItems::center;
    fb.justifyContent = FlexBox::JustifyContent::center;
    juce::FlexItem::Margin margin1 (0, 0, 0, -10);
    fb.items = {
        FlexItem(area.getWidth() * 0.4, area.getHeight(), col1).withMargin(margin1),
        FlexItem(area.getWidth() * 0.4, area.getHeight(), col2),
    };
    fb.performLayout(area.toFloat());
}

// Combined TabComponent //

LFOVibratoModule::LFOVibratoModule() {
    addAndMakeVisible(tabs);
    tabs.addTab("Filter LFO", juce::Colours::slategrey.darker(), &lfoTab, false);
    tabs.addTab("Vibrato", juce::Colours::slategrey, &vibratoTab, false);
}

LFOVibratoModule::~LFOVibratoModule() {}

void LFOVibratoModule::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::slategrey.darker().darker());
}

void LFOVibratoModule::resized() {
    auto area = getLocalBounds();
    tabs.setBounds(area);
}
