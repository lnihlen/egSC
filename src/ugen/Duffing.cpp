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
    int stepsPerSample;
    double step;

    // Tracks time relative to instantaneous frequency for the driving oscillator.
    double phase;

    // Displacement at time t and first derivative (velocity).
    double y, yPrime;
};

struct DuffingExt : public Unit {
    int stepsPerSample;
    float step;
    double y, yPrime;
    float x0, x1, x2, x3;
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

    // Numerical instability results for step sizes larger than this, but as this gets smaller the cost of the
    // compute per sample goes up, so the step size is derived experimentally to be as large as possible while
    // still stable.
    constexpr double kMaxStep = 1.0 / 2.0;
    unit->stepsPerSample = static_cast<int>(ceil(unit->h / kMaxStep));
    unit->step = unit->h > kMaxStep ? unit->h / ceil(unit->h / kMaxStep) : unit->h;

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
    int stepsPerSample = unit->stepsPerSample;
    double step = unit->step;
    double x = unit->phase;
    double y = unit->y;
    double yPrime = unit->yPrime;

    double angularPeriod = 100000.0 / freq;
    while (x >= angularPeriod) {
        x -= angularPeriod;
    }

    for (auto i = 0; i < inNumSamples; ++i) {
        out[i] = zapgremlins(static_cast<float>(y));

        for (auto j = 0; j < stepsPerSample; ++j) {
            double yNext, yPrimeNext;
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
        m_Driver(0.0),
        m_Damping(damping),
        m_Stiffness(stiffness),
        m_NonLinearity(nonLinearity) {
    }

    double operator()(double x, double y, double yPrime) const {
        return m_Driver - (m_Damping * yPrime) - (m_Stiffness * y) - (m_NonLinearity * y * y * y);
    }

    double m_Driver;
    double m_Damping;
    double m_Stiffness;
    double m_NonLinearity;
};

void DuffingExt_Ctor(DuffingExt* unit) {
    double samplePeriod = SAMPLEDUR * 100000.0;
    constexpr double kMaxStep = 1.0 / 4.0;
    unit->stepsPerSample = static_cast<int>(ceil(samplePeriod / kMaxStep));
    unit->step = 1.0 / ceil(samplePeriod / kMaxStep);
    unit->y = 0.0;
    unit->yPrime = 0.0;
    unit->x0 = 0.0;
    unit->x1 = 0.0;
    unit->x2 = 0.0;
    unit->x3 = 0.0;

    SETCALC(DuffingExt_next);
}

void DuffingExt_next(DuffingExt* unit, int inNumSamples) {
    float* out = OUT(0);
    float* in = IN(0);
    double damping = static_cast<double>(IN0(1));
    double stiffness = static_cast<double>(IN0(2));
    double nonLinearity = static_cast<double>(IN0(3));

    DuffingExtFunctor f(damping, stiffness, nonLinearity);

    int stepsPerSample = unit->stepsPerSample;
    float step = unit->step;
    double doubleStep = static_cast<double>(step);
    double y = unit->y;
    double yPrime = unit->yPrime;
    float x0 = unit->x0;
    float x1 = unit->x1;
    float x2 = unit->x2;
    float x3 = unit->x3;

    // Note 4 sample delay from input.
    for (auto i = 0; i < inNumSamples; ++i) {
        // Read must occur before write, as output and input buffers may be the same buffer.
        float in_i = in[i];
        out[i] = zapgremlins(static_cast<float>(y));

        float x = 0.0;
        for (auto j = 0; j < stepsPerSample; ++j) {
            double yNext, yPrimeNext;
            f.m_Driver = static_cast<double>(cubicinterp(x, x0, x1, x2, x3));
            egSC::LinearIntegrator<DuffingExtFunctor>(f, doubleStep, 0.0, y, yPrime, yNext, yPrimeNext);

            x += step;

            if (isnan(yNext) || isnan(yPrimeNext)) {
                y = 0.0;
                yPrime = 0.0;
            } else {
                y = yNext;
                yPrime = yPrimeNext;
            }
        }

        x0 = x1;
        x1 = x2;
        x2 = x3;
        x3 = in_i;
    }

    unit->y = y;
    unit->yPrime = yPrime;
    unit->x0 = x0;
    unit->x1 = x1;
    unit->x2 = x2;
    unit->x3 = x3;
}

