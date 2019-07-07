#ifndef SRC_UGEN_LINEAR_INTEGRATOR_HPP_
#define SRC_UGEN_LINEAR_INTEGRATOR_HPP_

namespace egSC {

// General second-order ODE integration solver, assuming linear integration from y'' to y' to y. Not likely to be
// accurate, particularly for nonsmooth ODEs, but as cheap as it gets at one function eval per sample.
template<typename ODE>
void LinearIntegrator(const ODE& f, const double h, const double x, const double y, const double yPrime, double& yOut,
    double& yPrimeOut) {
    double yDoublePrime = f(x, y, yPrime);
    yPrimeOut = yPrime + (yDoublePrime * h);
    yOut = y + (yPrimeOut * h);
}

}    // namespace egSC

#endif    // SRC_UGEN_LINEAR_INTEGRATOR_HPP_
