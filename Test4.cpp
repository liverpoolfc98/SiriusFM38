#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include "DiffusionGBM.h"
#include "MCOptionHedger1D.hpp"
#include "Time.h"
#include "BSM.h"

using namespace siriusFM;

int main(int argc, char** argv) {
    if (argc != 11) {
        std::cerr << "usage: mu sigma s0 put/call strike Tdays deltaAcc tauMin paths" << std::endl;
        return 1;
    }
    auto mu = atof(argv[1]), sigma = atof(argv[2]), s0 = atof(argv[3]);
    auto K = atol(argv[5]), Tdays = atol(argv[6]);
    double deltaAcc = atof(argv[7]);
    int tauMin = atoi(argv[8]), nPaths = atoi(argv[9]);
    bool useTime = bool(atoi(argv[10]));

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

    printf("PnL = %f\n", std::get<0>(stats));
    printf("err = %f\n", std::get<1>(stats));
    printf("minPnL = %f\n", std::get<2>(stats));
    printf("maxPnL = %f\n", std::get<3>(stats));

    delete option;

    return 0;
}
