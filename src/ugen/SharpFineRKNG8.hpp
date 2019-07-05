#ifndef SRC_SHARP_FINE_RKNG_8_H_
#define SRC_SHARP_FINE_RKNG_8_H_

namespace egSC {

template<typename ODE>
void SharpFineRKNG8(const ODE& f, const double h, const double x, const double y, const double yPrime,
    double& yOut, double& yPrimeOut, double& yHatOut, double& yHatPrimeOut) {
    constexpr double a_21 = 1.0 / 200.0;

    constexpr double a_31 = 14.0 / 2187.0;
    constexpr double a_32 = 40.0 / 2187.0;

    constexpr double a_41 = 148.0 / 3087.0;
    constexpr double a_42 = -85.0 / 3087.0;
    constexpr double a_43 = 1.0 / 14.0;

    constexpr double a_51 = -2201.0 / 28350.0;
    constexpr double a_52 = 932.0 / 2835.0;
    constexpr double a_53 = -7.0 / 50.0;
    constexpr double a_54 = 1.0 / 9.0;

    constexpr double a_61 = 13198826.0 / 54140625.0;
    constexpr double a_62 = -5602364.0 / 10828215.0;
    constexpr double a_63 = 278987101.0 / 44687500.0;
    constexpr double a_64 = -332539.0 / 4021872.0;
    constexpr double a_65 = 1.0 / 20.0;

    constexpr double a_71 = -601416947.0 / 16216200.0;
    constexpr double a_72 = 2972539.0 / 810810.0;
    constexpr double a_73 = 10883471.0 / 2574000.0;
    constexpr double a_74 = -503477.0 / 99000.0;
    constexpr double a_75 = 3.0 / 5.0;
    constexpr double a_76 = 4.0 / 5.0;

    constexpr double a_81 = -228527046421.0 / 72442188000.0;
    constexpr double a_82 = 445808287.0 / 139311900.0;
    constexpr double a_83 = 104724572891.0 / 29896776000.0;
    constexpr double a_84 = -31680158501.0 / 747419400.0;
    constexpr double a_85 = 1033813.0 / 2044224.0;
    constexpr double a_86 = 1166143.0 / 1703520.0;
    constexpr double a_87 = 0.0;

    constexpr double aPrime_21 = 1.0 / 10.0;

    constexpr double aPrime_31 = -2.0 / 81.0;
    constexpr double aPrime_32 = 20.0 / 81.0;

    constexpr double aPrime_41 = 615.0 / 1372.0;
    constexpr double aPrime_42 = -270.0 / 343.0;
    constexpr double aPrime_43 = 1053.0 / 1372.0;

    constexpr double aPrime_51 = 140.0 / 297.0;
    constexpr double aPrime_52 = -20.0 / 33.0;
    constexpr double aPrime_53 = 42.0 / 143.0;
    constexpr double aPrime_54 = 1960.0 / 3861.0;

    constexpr double aPrime_61 = -15544.0 / 20625.0;
    constexpr double aPrime_62 = 72.0 / 55.0;
    constexpr double aPrime_63 = 1053.0 / 6875.0;
    constexpr double aPrime_64 = -40768.0 / 103125.0;
    constexpr double aPrime_65 = 1521.0 / 3125.0;

    constexpr double aPrime_71 = 6841.0 / 1584.0;
    constexpr double aPrime_72 = -60.0 / 11.0;
    constexpr double aPrime_73 = -15291.0 / 7436.0;
    constexpr double aPrime_74 = 207319.0 / 33462.0;
    constexpr double aPrime_75 = -27.0 / 8.0;
    constexpr double aPrime_76 = 11125.0 / 8112.0;

    constexpr double aPrime_81 = 207707.0 / 54432.0;
    constexpr double aPrime_82 = -305.0 / 63.0;
    constexpr double aPrime_83 = -24163.0 / 14196.0;
    constexpr double aPrime_84 = 4448227.0 / 821240.0;
    constexpr double aPrime_85 = -4939.0 / 1680.0;
    constexpr double aPrime_86 = 3837625.0 / 3066336.0;
    constexpr double aPrime_87 = 0.0;

    constexpr double b_1 = 23.0 / 320.0;
    constexpr double b_2 = 0.0;
    constexpr double b_3 = 12393.0 / 54080.0;
    constexpr double b_4 = 2401.0 / 25350.0;
    constexpr double b_5 = 99.0 / 1600.0;
    constexpr double b_6 = 1375.0 / 32448.0;
    constexpr double b_7 = 0.0;
    constexpr double b_8 = 0.0;

    constexpr double bPrime_1 = 23.0 / 320.0;
    constexpr double bPrime_2 = 0.0;
    constexpr double bPrime_3 = 111537.0 / 378560.0;
    constexpr double bPrime_4 = 16807.0 / 101400.0;
    constexpr double bPrime_5 = 297.0 / 600.0;
    constexpr double bPrime_6 = 6875.0 / 32448.0;
    constexpr double bPrime_7 = -319.0 / 840.0;
    constexpr double bPrime_8 = 9.0 / 20.0;

    constexpr double bHat_1 = 22151.0 / 202500.0;
    constexpr double bHat_2 = 0.0;
    constexpr double bHat_3 = 64521 / 910000.0;
    constexpr double bHat_4 = 8536927.0 / 26325000.0;
    constexpr double bHat_5 = -16429.0 / 150000.0;
    constexpr double bHat_6 = 1.0 / 10.0;
    constexpr double bHat_7 = -3971.0 / 37800.0;
    constexpr double bHat_8 = 11.0 / 100.0;

    constexpr double bHatPrime_1 = 241.0 / 2880.0;
    constexpr double bHatPrime_2 = 0.0;
    constexpr double bHatPrime_3 = 12393.0 / 54080.0;
    constexpr double bHatPrime_4 = 45619.0 / 152100.0;
    constexpr double bHatPrime_5 = -9.0 / 1600.0;
    constexpr double bHatPrime_6 = 11125.0 / 32448.0;
    constexpr double bHatPrime_7 = 1.0 / 20.0;
    constexpr double bHatPrime_8 = 0.0;

    constexpr double c_1 = 1.0;
    constexpr double c_2 = 1.0 / 10.0;
    constexpr double c_3 = 2.0 / 9.0;
    constexpr double c_4 = 3.0 / 7.0;
    constexpr double c_5 = 2.0 / 3.0;
    constexpr double c_6 = 4.0 / 5.0;
    constexpr double c_7 = 1.0;
    constexpr double c_8 = 1.0;

    double h2 = h * h;

    double f_1 = f(x + (h * c_1), y + (h * c_1 * yPrime), yPrime);
    double f_2 = f(x + (h * c_2), y + (h * c_2 * yPrime) + (h2 * (a_21 * f_1)), yPrime + (h * (aPrime_21 * f_1)));
    double f_3 = f(x + (h * c_3), y + (h * c_3 * yPrime) + (h2 * ((a_31 * f_1) + (a_32 * f_2))), yPrime + (h *
        ((aPrime_31 * f_1) + (aPrime_32 * f_2))));
    double f_4 = f(x + (h * c_4), y + (h * c_4 * yPrime) + (h2 * ((a_41 * f_1) + (a_42 * f_2) + (a_43 * f_3))),
        yPrime + (h * ((aPrime_41 * f_1) + (aPrime_42 * f_2) + (aPrime_43 * f_3))));
    double f_5 = f(x + (h * c_5), y + (h * c_5 * yPrime) + (h2 * ((a_51 * f_1) + (a_52 * f_2) + (a_53 * f_3) + (a_54
        * f_4))), yPrime + (h * ((aPrime_51 * f_1) + (aPrime_52 * f_2) + (aPrime_53 * f_3) + (aPrime_54 * f_4))));
    double f_6 = f(x + (h * c_6), y + (h * c_6 * yPrime) + (h2 * ((a_61 * f_1) + (a_62 * f_2) + (a_63 * f_3) + (a_64
        * f_4) + (a_65 * f_5))), yPrime + (h * ((aPrime_61 * f_1) + (aPrime_62 * f_2) + (aPrime_63 * f_3) + (aPrime_64 *
        f_4) + (aPrime_65 * f_5))));
    double f_7 = f(x + (h * c_7), y + (h * c_7 * yPrime) + (h2 * ((a_71 * f_1) + (a_72 * f_2) + (a_73 * f_3) + (a_74
        * f_4) + (a_75 * f_5) + (a_76 * f_6))), yPrime + (h * ((aPrime_71 * f_1) + (aPrime_72 * f_2) + (aPrime_73 * f_3)
        + (aPrime_74 * f_4) + (aPrime_75 * f_5) + (aPrime_76 * f_6))));
    double f_8 = f(x + (h * c_8), y + (h * c_8 * yPrime) + (h2 * ((a_81 * f_1) + (a_82 * f_2) + (a_83 * f_3) + (a_84
        * f_4) + (a_85 * f_5) + (a_86 * f_6) + (a_87 * f_7))), yPrime + (h * ((aPrime_81 * f_1) + (aPrime_82 * f_2) +
        (aPrime_83 * f_3) + (aPrime_84 * f_4) + (aPrime_85 * f_5) + (aPrime_86 * f_6) + (aPrime_87 * f_7))));

    yOut = y + (h * yPrime) + (h2 * ((b_1 * f_1) + (b_2 * f_2) + (b_3 * f_3) + (b_4 * f_4) + (b_5 * f_5) + (b_6 * f_6) +
        (b_7 * f_7) + (b_8 * f_8)));
    yPrimeOut = yPrime + (h * ((bPrime_1 * f_1) + (bPrime_2 * f_2) + (bPrime_3 * f_3) + (bPrime_4 * f_4) + (bPrime_5 *
        f_5) + (bPrime_6 * f_6) + (bPrime_7 * f_7) + (bPrime_8 * f_8)));
    yHatOut = y + (h * yPrime) + (h2 * ((bHat_1 * f_1) + (bHat_2 * f_2) + (bHat_3 * f_3) + (bHat_4 * f_4) + (bHat_5 *
        f_5) + (bHat_6 * f_6) + (bHat_7 * f_7) + (bHat_8 * f_8)));
    yHatPrimeOut = yPrime + (h * ((bHatPrime_1 * f_1) + (bHatPrime_2 * f_2) + (bHatPrime_3 * f_3) + (bHatPrime_4 * f_4)
        + (bHatPrime_5 * f_5) + (bHatPrime_6 * f_6) + (bHatPrime_7 * f_7) + (bHatPrime_8 * f_8)));
}

}    // namespace egSC

#endif    // SRC_SHARP_FINE_RKNG_8_H_
