#pragma once
#include <JuceHeader.h>

/**
    A combination of an oscillator and a gain.
*/
template <typename Type>
class CustomOscillator {
    public:
        enum Waveform {
            sawtooth = 0,
            square,
            sine,
            noise
        };
    
        CustomOscillator() {
            auto& osc = processorChain.template get<oscIndex>();
            osc.initialise ([] (Type x) { return std::sin (x); }, 128);
        }

        void setFrequency (Type newValue, bool force = false) {
            auto& osc = processorChain.template get<oscIndex>();
            osc.setFrequency (newValue, force);
        }

        void setLevel (Type newValue) {
            auto& gain = processorChain.template get<gainIndex>();
            gain.setGainLinear (newValue);
        }

        void reset() noexcept {
            processorChain.reset();
        }

        template <typename ProcessContext>
        void process (const ProcessContext& context) noexcept {
            processorChain.process (context);
        }

        void prepare (const juce::dsp::ProcessSpec& spec) {
            processorChain.prepare (spec);
        }
    
        void setWaveform(const int waveformId) {
            auto& osc = processorChain.template get<oscIndex>();
            switch (waveformId) {
                case 0:
                    osc.initialise([] (Type x) { return juce::jmap (x, Type (-juce::MathConstants<double>::pi), Type (juce::MathConstants<double>::pi), Type (-1), Type (1)); }, 2); // saw
                    break;
                case 1:
                    osc.initialise ([] (Type x) { return x < 0.0f ? -1.0f : 1.0f; }, 128); // square
                    break;
                case 2:
                    osc.initialise([](Type x) { // triangle
                        if (x < 0.0f) {
                            return juce::jmap (x, Type (-juce::MathConstants<double>::pi), 0.0f, Type (-1), Type (1));
                        } else {
                            return juce::jmap (x, 0.0f, Type (juce::MathConstants<double>::pi), Type (1), Type (-1));
                        }
                    }, 3);
                    break;
                case 3:
                    osc.initialise ([this](Type x) { return 2.0f * random.nextFloat() - 1.0f; }); // white noise
                    break;
                default:
                    jassertfalse;
                    break;
            }
        }

    private:
        enum {
            oscIndex,
            gainIndex
        };
        /// A Random object for generating random numbers.
        Random random;
    
        juce::dsp::ProcessorChain<juce::dsp::Oscillator<Type>, juce::dsp::Gain<Type>> processorChain;
};
