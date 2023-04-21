#pragma once
#include <JuceHeader.h>

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
                    osc.initialise([] (Type x) { return juce::jmap (x, Type (-juce::MathConstants<double>::pi), Type (juce::MathConstants<double>::pi), Type (-1), Type (1)); }, 2);
                    break;
                case 1:
                    osc.initialise ([] (Type x) { return x < 0.0f ? -1.0f : 1.0f; }, 128); // square
                    break;
                case 2:
                    osc.initialise ([](Type x) { return std::sin (x); }, 128); // tri
                    break;
                case 3:
                    osc.initialise ([](Type x) { return std::sin (x); }, 128);
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
        juce::dsp::ProcessorChain<juce::dsp::Oscillator<Type>, juce::dsp::Gain<Type>> processorChain;
};
