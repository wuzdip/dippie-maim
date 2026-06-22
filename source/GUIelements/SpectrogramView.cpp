//
// SpectrogramView.cpp
//

#include "SpectrogramView.h"

SpectrogramView::SpectrogramView(SpectrogramDataSource& source) : dataSource(source)
{
    setOpaque(true);
    spectrogramImage = juce::Image(juce::Image::RGB, 512, 300, true);
    startTimerHz(30);
}

void SpectrogramView::timerCallback()
{
    if (dataSource.getFFTDataIfReady(fftData)) {
        drawNextLineOfSpectrogram();
        repaint();
    }
}

void SpectrogramView::paint(juce::Graphics& g)
{
    g.drawImage(spectrogramImage, getLocalBounds().toFloat());
}

void SpectrogramView::resized()
{
}

void SpectrogramView::drawNextLineOfSpectrogram()
{
    auto rightHandEdge = spectrogramImage.getWidth() - 1;
    auto imageHeight = spectrogramImage.getHeight();

    spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);

    forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());

    auto maxLevel = juce::FloatVectorOperations::findMinAndMax(fftData.data(), SpectrogramDataSource::fftSize / 2);
    auto maxValue = juce::jmax(maxLevel.getEnd(), 1e-5f);

    for (auto y = 1; y < imageHeight; ++y) {
        auto skewedProportionY = 1.0f - std::exp(std::log((float) y / (float) imageHeight) * 0.2f);
        auto fftDataIndex = (size_t) juce::jlimit(0, SpectrogramDataSource::fftSize / 2,
            (int) (skewedProportionY * (float) SpectrogramDataSource::fftSize / 2.f));
        auto level = juce::jmap(fftData[fftDataIndex], 0.0f, maxValue, 0.0f, 1.0f);
        spectrogramImage.setPixelAt(rightHandEdge, y, juce::Colour::fromHSV(level, 1.0f, level, 1.0f));
    }
}
