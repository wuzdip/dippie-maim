//
// SpectrogramView.h
//
// Scrolling frequency-over-time view of the plugin's final output audio.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_graphics/juce_graphics.h>

#include "../SpectrogramDataSource.h"

class SpectrogramView : public juce::Component, private juce::Timer
{
public:
    explicit SpectrogramView(SpectrogramDataSource& source);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawNextLineOfSpectrogram();

    SpectrogramDataSource& dataSource;
    juce::dsp::FFT forwardFFT { SpectrogramDataSource::fftOrder };
    juce::Image spectrogramImage;
    std::array<float, SpectrogramDataSource::fftSize * 2> fftData {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramView)
};
