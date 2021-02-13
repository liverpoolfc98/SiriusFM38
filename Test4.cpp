#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include "DiffusionGBM.h"
#include "MCOptionHedger1D.hpp"
#include "Time.h"

namespace siriusFM {
    double Phi(double x) {
        return 0.5 * (1 + erf(x / M_SQRT2));
    }
    double BSMPriceCall(double a_s0, double a_K, double a_Ty,
        double a_sigma, double a_rateA, double a_rateB) {
        assert(a_s0 > 0 && a_K > 0 && a_sigma > 0);
        double x1 = (std::log(a_s0 / a_K) + a_Ty * (a_rateB - a_rateA
            + a_sigma * a_sigma / 2)) / a_sigma / sqrt(a_Ty);
        double x2 = (std::log(a_s0 / a_K) + a_Ty * (a_rateB - a_rateA
            - a_sigma * a_sigma / 2)) / a_sigma / sqrt(a_Ty);
        return a_s0 * std::exp(-a_rateA * a_Ty) * Phi(x1)
            - a_K * std::exp(-a_rateB * a_Ty) * Phi(x2);
    }
    double BSMPricePut(double a_s0, double a_K, double a_Ty,
        double a_sigma, double a_rateA, double a_rateB) {
        assert(a_s0 > 0 && a_K > 0 && a_sigma > 0);
        return -a_s0 + a_K * std::exp(-a_rateB * a_Ty)
            + BSMPriceCall(a_s0, a_K, a_Ty, a_sigma, a_rateA, a_rateB);
    }
    double DeltaCall(double a_s0, double a_K, double a_Ty,
        double a_sigma, double a_rateA, double a_rateB) {
        assert(a_s0 > 0 && a_K > 0 && a_sigma > 0);
        double x1 = (std::log(a_s0 / a_K) + a_Ty * (a_rateB - a_rateA
            + a_sigma * a_sigma / 2)) / a_sigma / sqrt(a_Ty);
        return Phi(x1);
    }
    double DeltaPut(double a_s0, double a_K, double a_Ty,
        double a_sigma, double a_rateA, double a_rateB) {
        return DeltaCall(a_s0, a_K, a_Ty, a_sigma, a_rateA, a_rateB) - 1;
    }
}

using namespace siriusFM;

int main(int argc, char** argv) {
    if (argc != 11) {
        std::cerr << "usage: mu sigma s0 put/call strike Tdays deltaAcc tauMin paths" << std::endl;
        return 1;
    }
    auto mu = atof(argv[1]), sigma = atof(argv[2]), s0 = atof(argv[3]);
    auto K = atol(argv[5]), Tdays = atol(argv[6]);
    bool useTime = bool(atoi(argv[10]));
    int tauMin = atoi(argv[8]), nPaths = atoi(argv[9]);
    double deltaAcc = atof(argv[7]);

    auto t0 = time(nullptr);

    OptionFX* option;

    DiffusionGBM diffGBM(mu, sigma, s0);
    MCOptionHedger1D<DiffusionGBM, IRProvider<IRModeE::Const>, IRProvider<IRModeE::Const>,
    CcyE, CcyE> hedger(nullptr, nullptr, &diffGBM, useTime);

    double rateA = hedger.getRateA(CcyE::EUR, t0), rateB = hedger.getRateB(CcyE::EUR, t0);

    decltype(hedger)::DeltaFunc const* deltaFunc = nullptr;

    if (strcmp(argv[4], "Put") == 0) {
        option = new EurPutOptionFX(K, time(nullptr) + Tdays * 24 * 60 * 60, CcyE::USD, CcyE::USD);
    } else if (strcmp(argv[4], "Call") == 0) {
        option = new EurCallOptionFX(K, time(nullptr) + Tdays * 24 * 60 * 60, CcyE::USD, CcyE::USD);
    } else {
        throw std::invalid_argument("Put or Call required");
    }


    double Ty = YearFrac(option->expirTime());
    double c0;

    std::function<double(double, double)> deltaCall(
        [&](double a_s, double a_t) {
            return DeltaCall(a_s, K, Ty - a_t, sigma, rateA, rateB);
        }
    );

    std::function<double(double, double)> deltaPut(
        [&](double a_s, double a_t) {
            return DeltaPut(a_s, K, Ty - a_t, sigma, rateA, rateB);
        }
    );

    if (strcmp(argv[4], "Put") == 0) {
        c0 = BSMPricePut(s0, K, Ty - YearFrac(t0), sigma, rateA, rateB);
        deltaFunc = &deltaPut;
    } else if (strcmp(argv[4], "Call") == 0) {
        c0 = BSMPriceCall(s0, K, Ty - YearFrac(t0), sigma, rateA, rateB);
        deltaFunc = &deltaCall;
    } else {
        throw std::invalid_argument("Put or Call required");
    }

    auto stats = hedger.SimulateHedging(option, t0, tauMin, nPaths,
        c0, deltaFunc, deltaAcc);

    printf("PNL = %f\n", std::get<0>(stats));

    delete option;

    return 0;
}
