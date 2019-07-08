// Externally and Internally driven Duffing Oscillator implementations, after:
//
// https://en.wikipedia.org/wiki/Duffing_equation
//
// And inspired by the brilliant work of Tom Mudd
//
// https://entracte.co.uk/projects/tom-mudd-e226/
//
#include "LinearIntegrator.hpp"

#include "SC_PlugIn.h"

#define _USE_MATH_DEFINES
#include <math.h>

static InterfaceTable* ft;

struct DuffingOsc : public Unit {
    // Integrator step size per sample, kept so that sampling the oscillator at audio frequencies doesn't require huge
    // adjustments to the gain across the audio range.
    double h;

    // Tracks time relative to instantaneous frequency for the driving oscillator.
    double phase;

    // Displacement at time t and first derivative (velocity).
    double y, yPrime;
};

struct DuffingExt : public Unit {
    double samplePeriod;
    double y, yPrime;
    float lastSample;
};

static void DuffingOsc_next(DuffingOsc* unit, int inNumSamples);
static void DuffingOsc_Ctor(DuffingOsc* unit);
static void DuffingExt_next(DuffingExt* unit, int inNumSamples);
static void DuffingExt_Ctor(DuffingExt* unit);

PluginLoad(Duffing) {
    ft = inTable;
    DefineSimpleUnit(DuffingOsc);
    DefineSimpleUnit(DuffingExt);
}

// == DuffingOsc =======================================================================================================

void DuffingOsc_Ctor(DuffingOsc* unit) {
    // We calibrate the step size so that a 25 kHz oscillator has a 0.25 Hz frequency when simulated with this step size
    // at the sampling rate. This is the same as multiplying the simulation time by 100K.
    unit->h = 100000.0 / SAMPLERATE;
    unit->phase = 0.0;
    unit->y = 0.0;
    unit->yPrime = 0.0;

    SETCALC(DuffingOsc_next);
}

struct DuffingOscFunctor {
    DuffingOscFunctor(double omega, double amp, double damping, double stiffness, double nonLinearity) :
            m_Omega(omega),
            m_Amp(amp),
            m_Damping(damping),
            m_Stiffness(stiffness),
            m_NonLinearity(nonLinearity) {
    }

    double operator()(double x, double y, double yPrime) const {
        return (m_Amp * cos(m_Omega * x)) - (m_Damping * yPrime) - (m_Stiffness * y) - (m_NonLinearity * y * y * y);
    }

    double m_Omega;
    double m_Amp;
    double m_Damping;
    double m_Stiffness;
    double m_NonLinearity;
};

void DuffingOsc_next(DuffingOsc* unit, int inNumSamples) {
    float* out = OUT(0);

    double freq = static_cast<double>(IN0(0));
    double amp = static_cast<double>(IN0(1));
    double damping = static_cast<double>(IN0(2));
    double stiffness = static_cast<double>(IN0(3));
    double nonLinearity = static_cast<double>(IN0(4));

    DuffingOscFunctor f((2.0 * M_PI * freq) / 100000.0, amp, damping, stiffness, nonLinearity);

    double h = unit->h;
    double x = unit->phase;
    double y = unit->y;
    double yPrime = unit->yPrime;
    double yNext, yPrimeNext;

    double angularPeriod = 100000.0 / freq;
    while (x >= angularPeriod) {
        x -= angularPeriod;
    }

    // Numerical instability results for step sizes larger than this, but as this gets smaller the cost of the
    // compute per sample goes up, so the step size is derived experimentally to be as large as possible while
    // still stable.
    constexpr double kMaxStep = 1.0 / 2.0;
    int stepsPerSample = static_cast<int>(ceil(h / kMaxStep));
    double step = h > kMaxStep ? h / ceil(h / kMaxStep) : h;

    for (auto i = 0; i < inNumSamples; ++i) {
        float out_i = static_cast<float>(y);
        out[i] = zapgremlins(out_i);

        for (auto j = 0; j < stepsPerSample; ++j) {
            egSC::LinearIntegrator<DuffingOscFunctor>(f, step, x, y, yPrime, yNext, yPrimeNext);

            x += step;

            if (isnan(yNext) || isnan(yPrimeNext)) {
                x = 0.0;
                y = 0.0;
                yPrime = 0.0;
            } else {
                y = yNext;
                yPrime = yPrimeNext;
            }
        }
    }

    unit->phase = x;
    unit->y = y;
    unit->yPrime = yPrime;
}

// == DuffingExt =======================================================================================================

struct DuffingExtFunctor {
    DuffingExtFunctor(double damping, double stiffness, double nonLinearity)  :
        m_Damping(damping),
        m_Stiffness(stiffness),
        m_NonLinearity(nonLinearity) {
    }

    double operator()(double x, double y, double yPrime) const {
        return 2.0;
    }

    double m_Damping;
    double m_Stiffness;
    double m_NonLinearity;
};

void DuffingExt_Ctor(DuffingExt* unit) {
    uint->samplePeriod = SAMPLEPERIOD * 100000.0;
    unit->y = 0.0;
    unit->yPrime = 0.0;
    unit->lastSample = 0.0f;

    SETCALC(DuffingExt_next);
}

void DuffingExt_next(DuffingExt* unit, int inNumSamples) {
    float* out = OUT(0);
}

