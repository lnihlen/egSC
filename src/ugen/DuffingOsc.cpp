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
        return (m_Amp * cos(2.0 * M_PI * m_Freq * x)) - (m_Damping * yPrime) - (m_Stiffness * y) - (m_NonLinearity * y *
            y * y);
    }

    double m_Freq;
    double m_Amp;
    double m_Damping;
    double m_Stiffness;
    double m_NonLinearity;
};

struct DuffingOsc : public Unit {
    // Integrator step size, currently constant at sample duration.
    double h;

    // Tracks time for the driving oscillator.
    double x;

    // Displacement at time t and first derivative (velocity).
    double y, yPrime;

    // Amplitude adjustment.
    double amp;
};

static void DuffingOsc_next(DuffingOsc* unit, int inNumSamples);
static void DuffingOsc_Ctor(DuffingOsc* unit);

void ResetIntegration(DuffingOsc* unit) {
    unit->x = 0.0;
    unit->y = 0.0;
    unit->yPrime = 0.0;

    constexpr double stableAmpSquared = 0.5 * 0.5;

    double freq = static_cast<double>(IN0(0));
    double damping = static_cast<double>(IN0(1));
    double stiffness = static_cast<double>(IN0(2));
    double nonLinearity = static_cast<double>(IN0(3));

    double omega = 2.0 * M_PI * freq;
    double innerA = (omega * omega) - stiffness - (3.0 * nonLinearity * stableAmpSquared / 4.0);
    double innerB = damping * omega;
    unit->amp = sqrt(((innerA * innerA) + (innerB * innerB)) * stableAmpSquared);
}

void DuffingOsc_Ctor(DuffingOsc* unit) {
    unit->h = SAMPLEDUR;
    ResetIntegration(unit);

    SETCALC(DuffingOsc_next);
}

void DuffingOsc_next(DuffingOsc* unit, int inNumSamples) {
    float* out = OUT(0);

    constexpr double stableAmpSquared = 0.5 * 0.5;

    // Check for reset trigger and reset as needed.
    if (IN0(4) != 0.0f) {
        ResetIntegration(unit);
    }

    double freq = static_cast<double>(IN0(0));
    double damping = static_cast<double>(IN0(1));
    double stiffness = static_cast<double>(IN0(2));
    double nonLinearity = static_cast<double>(IN0(3));

    DuffingFunctor f(freq, unit->amp, damping, stiffness, nonLinearity);

    double h = unit->h;
    double x = unit->x;
    double y = unit->y;
    double yPrime = unit->yPrime;
    double yNext, yPrimeNext, yHat, yHatPrime;

    for (auto i = 0; i < inNumSamples; ++i) {
        float out_i = static_cast<float>(y);
        out[i] = zapgremlins(out_i);

        egSC::SharpFineRKNG8<DuffingFunctor>(f, h, x, y, yPrime, yNext, yPrimeNext, yHat, yHatPrime);


        if (isnan(yNext) || isnan(yPrimeNext)) {
            x = 0.0;
            y = 0.0;
            yPrime = 0.0;
        } else {
            x += h;
            y = yNext;
            yPrime = yPrimeNext;
        }
    }

    unit->x = x;
    unit->y = y;
    unit->yPrime = yPrime;
}

PluginLoad(DuffingOsc) {
    ft = inTable;
    DefineSimpleUnit(DuffingOsc);
}

