// SPDX-License-Identifier: GPL-3.0-only

// This is derived from the VCV-Rack Fundamental VCF
// by Andrew Belt and contributors

#pragma once
#include <juce_core/juce_core.h>
#include <cmath>

class HeraVCF_Rack {
    using T = float;

    T state[4];
    T input;
    T sampleRate = 44100;

public:
    HeraVCF_Rack()
    {
        reset();
    }

    void setSampleRate(float newRate)
    {
        sampleRate = newRate;
    }

    void reset()
    {
        for (int i = 0; i < 4; i++)
            state[i] = 0;
        input = 0;
    }

    void processNextBlock(float *inputAndOutput, const float *cutoffs, const float *resonances, int numSamples)
    {
        T input = this->input;
        T dt = T(1) / sampleRate;

        for (int i = 0; i < numSamples; ++i) {
            T prevInput = input;
            input = inputAndOutput[i];
            T omega0 = 2 * T(M_PI) * juce::jmin(20000.0f, cutoffs[i]);
            T resonance = 5.0f * resonances[i];

            stepRK4<4>(T(0), dt, state, [dt, prevInput, input, omega0, resonance](T t, const T x[], T dxdt[]) {
                T inputt = crossfade(prevInput, input, t / dt);
                T inputc = clip(inputt - resonance * x[3]);
                T yc0 = clip(x[0]);
                T yc1 = clip(x[1]);
                T yc2 = clip(x[2]);
                T yc3 = clip(x[3]);

                dxdt[0] = omega0 * (inputc - yc0);
                dxdt[1] = omega0 * (yc0 - yc1);
                dxdt[2] = omega0 * (yc1 - yc2);
                dxdt[3] = omega0 * (yc2 - yc3);
            });

            inputAndOutput[i] = state[3];
        }

        this->input = input;
    }

    template <int len, typename T, typename F>
    void stepRK4(T t, T dt, T x[], F f) {
        T k1[len];
        T k2[len];
        T k3[len];
        T k4[len];
        T yi[len];

        f(t, x, k1);

        for (int i = 0; i < len; i++) {
            yi[i] = x[i] + k1[i] * dt / 2.f;
        }
        f(t + dt / 2.f, yi, k2);

        for (int i = 0; i < len; i++) {
            yi[i] = x[i] + k2[i] * dt / 2.f;
        }
        f(t + dt / 2.f, yi, k3);

        for (int i = 0; i < len; i++) {
            yi[i] = x[i] + k3[i] * dt;
        }
        f(t + dt, yi, k4);

        for (int i = 0; i < len; i++) {
            x[i] += dt * (k1[i] + 2.f * k2[i] + 2.f * k3[i] + k4[i]) / 6.f;
        }
    }

private:
    static inline T clip(T x)
    {
        // Pade approximant of tanh
        x = (x < -3.f) ? -3.f : x;
        x = (x > +3.f) ? +3.f : x;
        return x * (27 + x * x) / (27 + 9 * x * x);
    }

    static inline T crossfade(T a, T b, T p) {
        return a + (b - a) * p;
    }
};
