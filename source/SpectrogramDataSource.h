//
// SpectrogramDataSource.h
//
// Lock-free single-producer/single-consumer hand-off of audio-thread output
// samples to the UI thread for spectrogram display. Audio thread calls
// pushSample() continuously; UI thread polls getFFTDataIfReady() on a timer.
//

#pragma once

#include <algorithm>
#include <array>
#include <atomic>

class SpectrogramDataSource
{
public:
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;

    void pushSample(float sample)
    {
        if (fifoIndex == fftSize) {
            if (!nextBlockReady.load()) {
                std::fill(fftData.begin(), fftData.end(), 0.f);
                std::copy(fifoBuffer.begin(), fifoBuffer.end(), fftData.begin());
                nextBlockReady.store(true);
            }
            fifoIndex = 0;
        }
        fifoBuffer[(size_t) fifoIndex++] = sample;
    }

    bool getFFTDataIfReady(std::array<float, fftSize * 2>& destination)
    {
        if (!nextBlockReady.load()) {
            return false;
        }
        destination = fftData;
        nextBlockReady.store(false);
        return true;
    }

private:
    std::array<float, fftSize> fifoBuffer {};
    std::array<float, fftSize * 2> fftData {};
    int fifoIndex = 0;
    std::atomic<bool> nextBlockReady { false };
};
