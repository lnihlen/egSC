// Duffing Oscillator implementation, after:
//
// https://en.wikipedia.org/wiki/Duffing_equation
//
// And inspired by the brilliant work of Tom Mudd
//
// https://entracte.co.uk/projects/tom-mudd-e226/
//
#include "LinearIntegrator.hpp"
#include "SharpFineRKNG8.hpp"

#include "SC_PlugIn.h"

#define _USE_MATH_DEFINES
#include <math.h>

static InterfaceTable* ft;

struct DuffingFunctor {
    DuffingFunctor(double freq, double damping, double stiffness, double nonLinearity) :
            m_Omega(2.0 * M_PI * (freq / 100000.0)),
            m_Damping(damping),
            m_Stiffness(stiffness),
            m_NonLinearity(nonLinearity) {
    }

    // The 0.3 multiplier on the cos driver comes from an analysis of the steady-state amplitude function of the
    // Duffing equation provided in the Wikipedia page. Assuming a max frequency of 0.25, and all other inputs in
    // reasonable ranges, and keeping the desired steady-state amplitude at 0.5 results in a range of values for
    // the amplitude at right around 0.3.
    double operator()(double x, double y, double yPrime) const {
        return (0.3 * cos(m_Omega * x)) - (m_Damping * yPrime) - (m_Stiffness * y) - (m_NonLinearity * y * y * y);
    }

    double m_Omega;
    double m_Damping;
    double m_Stiffness;
    double m_NonLinearity;
};

struct DuffingOsc : public Unit {
    // Integrator step size, kept so that sampling the oscillator at audio frequencies doesn't require huge adjustments
    // to the gain across the audio range.
    double h;

    // Tracks time for the driving oscillator.
    double x;

    // Displacement at time t and first derivative (velocity).
    double y, yPrime;
};

static void DuffingOsc_next(DuffingOsc* unit, int inNumSamples);
static void DuffingOsc_Ctor(DuffingOsc* unit);

void ResetIntegration(DuffingOsc* unit) {
    unit->x = 0.0;
    unit->y = 0.0;
    unit->yPrime = 0.0;
}

void DuffingOsc_Ctor(DuffingOsc* unit) {
    // We calibrate the step size so that a 25 kHz oscillator has a 0.25 Hz frequency at the sampling rate.
    unit->h = 25000.0 / (0.25 * SAMPLERATE);
    ResetIntegration(unit);

    SETCALC(DuffingOsc_next);
}

void DuffingOsc_next(DuffingOsc* unit, int inNumSamples) {
    float* out = OUT(0);

    // Check for reset trigger and reset as needed.
    if (IN0(4) != 0.0f) {
        ResetIntegration(unit);
    }

    double freq = static_cast<double>(IN0(0));
    double damping = static_cast<double>(IN0(1));
    double stiffness = static_cast<double>(IN0(2));
    double nonLinearity = static_cast<double>(IN0(3));
    bool useAdvancedIntegrator = (IN0(5) != 0);

    DuffingFunctor f(freq, damping, stiffness, nonLinearity);

    double h = unit->h;
    double x = unit->x;
    double y = unit->y;
    double yPrime = unit->yPrime;
    double yNext, yPrimeNext, yHat, yHatPrime;

    // If statement is factored out of middle of loop to aid pipelining, but resulting in some redundant code.
    if (useAdvancedIntegrator) {
        constexpr double kMaxStep = 1.0 / 4.0;
        int stepsPerSample = static_cast<int>(ceil(h / kMaxStep));
        double step = h > kMaxStep ? h / ceil(h / kMaxStep) : h;

        for (auto i = 0; i < inNumSamples; ++i) {
            float out_i = static_cast<float>(y);
            out[i] = zapgremlins(out_i);

            for (auto j = 0; j < stepsPerSample; ++j) {
                egSC::SharpFineRKNG8<DuffingFunctor>(f, step, x, y, yPrime, yNext, yPrimeNext, yHat, yHatPrime);

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
    } else {
        constexpr double kMaxStep = 1.0 / 2.0;
        int stepsPerSample = static_cast<int>(ceil(h / kMaxStep));
        double step = h > kMaxStep ? h / ceil(h / kMaxStep) : h;

        for (auto i = 0; i < inNumSamples; ++i) {
            float out_i = static_cast<float>(y);
            out[i] = zapgremlins(out_i);

            for (auto j = 0; j < stepsPerSample; ++j) {
                egSC::LinearIntegrator<DuffingFunctor>(f, step, x, y, yPrime, yNext, yPrimeNext);

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
    }

    unit->x = x;
    unit->y = y;
    unit->yPrime = yPrime;
}

PluginLoad(DuffingOsc) {
    ft = inTable;
    DefineSimpleUnit(DuffingOsc);
}

