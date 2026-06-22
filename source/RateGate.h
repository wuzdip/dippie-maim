/*
  RateGate.h

  Shared timing engine for the tempo-syncable glitch effects (Speed, Stutter).
  Tracks a phase position through a repeating cycle whose length comes from
  either a free-running Hz value or a host-BPM-synced musical division, and
  reports whether the current audio block falls in the "active" portion of
  that cycle based on a duty-cycle fraction.
*/

#pragma once

#include <algorithm>
#include <array>

#include "parameterIds.h"

class RateGate
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        phaseSamples = 0.0;
    }

    void setBpm(double newBpm)
    {
        bpm = newBpm > 0.0 ? newBpm : 120.0;
    }

    void setSynced(bool shouldSync) { synced = shouldSync; }
    void setFreeRateHz(float hz) { freeRateHz = hz; }
    void setSyncedDivisionIndex(int index)
    {
        syncedDivisionIndex = std::clamp(index, 0, (int) SYNCED_RATE_BEATS.size() - 1);
    }

    // Advances the gate by numSamples and reports whether the block that was
    // just advanced through falls in the "active" (duty-cycle) portion.
    // dutyCycle: fraction (0-1) of each cycle considered "active."
    bool advanceFrameAndIsActive(int numSamples, float dutyCycle)
    {
        const double period = periodSamples();
        phaseSamples += (double) numSamples;
        while (phaseSamples >= period) {
            phaseSamples -= period;
        }
        return phaseSamples < (dutyCycle * period);
    }

    double periodSamples() const
    {
        double periodSeconds;
        if (synced) {
            const float beats = SYNCED_RATE_BEATS[(size_t) syncedDivisionIndex];
            periodSeconds = beats * (60.0 / bpm);
        } else {
            const float hz = std::max(freeRateHz, 0.01f);
            periodSeconds = 1.0 / hz;
        }
        // Clamp to a sane range regardless of tempo/rate extremes, so any
        // buffers sized off this (e.g. Stutter's capture buffer) stay bounded.
        periodSeconds = std::clamp(periodSeconds, 0.01, maxPeriodSeconds);
        return periodSeconds * sampleRate;
    }

    static constexpr double maxPeriodSeconds = 4.0;

private:
    double sampleRate = 48000.0;
    double bpm = 120.0;
    bool synced = false;
    float freeRateHz = 4.f;
    int syncedDivisionIndex = 4;

    double phaseSamples = 0.0;
};
