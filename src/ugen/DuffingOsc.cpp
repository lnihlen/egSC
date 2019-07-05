// Duffing Oscillator implementation, after:
// <ref>

#include "SharpFineRKNG8.hpp"

#include "SC_PlugIn.h"

#define _USE_MATH_DEFINES
#include <math.h>

static InterfaceTable* ft;

struct DuffingFunctor {
    DuffingFunctor(double freq, double amp, double damping, double stiffness, double nonLinearity) :
            m_Freq(freq),
            m_Amp(amp),
            m_Damping(damping),
            m_Stiffness(stiffness),
            m_NonLinearity(nonLinearity) {
    }

    double operator()(double x, double y, double yPrime) const {
        return (m_Damping * yPrime) + (m_Stiffness * y) + (m_NonLinearity * y * y * y) - (m_Amp * cos(2.0 * M_PI *
            m_Freq * x));
    }

    double m_Freq;
    double m_Amp;
    double m_Damping;
    double m_Stiffness;
    double m_NonLinearity;
};

struct DuffingOsc : public Unit {
    // Tracks time for the driving oscillator.
    double x;

    // Displacement at time t and first derivative (velocity).
    double y, yPrime;
};

extern "C" {
    void load(InterfaceTable* table);
    void DuffingOsc_Ctor(DuffingOsc* unit);
    void DuffingOsc_next(DuffingOsc* unit, int numSamples);
}

void load(InterfaceTable* table) {
    ft = table;
    DefineSimpleUnit(DuffingOsc);
}

void DuffingOsc_Ctor(DuffingOsc* unit) {
    unit->x = 0.0;
    unit->y = 0.0;
    unit->yPrime = 0.0;

    SETCALC(DuffingOsc_next);
    DuffingOsc_next(unit, 1);
}

void DuffingOsc_next(DuffingOsc* unit, int numSamples) {
    float* out = OUT(0);

    DuffingFunctor f(static_cast<double>(IN0(0)), static_cast<double>(IN0(1)), static_cast<double>(IN0(2)),
        static_cast<double>(IN0(3)), static_cast<double>(IN0(4)));

    double h = SAMPLEDUR;
    double x = unit->x;
    double y = unit->y;
    double yPrime = unit->yPrime;
    double yNext, yPrimeNext, yHat, yHatPrime;

    for (auto i = 0; i < numSamples; ++i) {
        out[i] = static_cast<float>(y);

        egSC::SharpFineRKNG8(f, h, x, y, yPrime, yNext, yPrimeNext, yHat, yHatPrime);

        x += h;
        y = yNext;
        yPrime = yPrimeNext;
    }

    unit->x = x;
    unit->y = y;
    unit->yPrime = yPrime;
}

