#pragma GCC optimize("Ofast,no-stack-protector")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,mmx,tune=native")

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <chrono>
#include <cassert>

#include "IRProviderConst.h"
#include "IRProvider.h"
#include "DiffusionGBM.h"
#include "DiffusionCEV.h"
#include "DiffusionCIR.h"
#include "DiffusionLipton.h"
#include "DiffusionOU.h"
#include "MCEngine1D.hpp"

using namespace siriusFM;

void TestGBM(double mu, double sigma, double s0, long T, int tauMin, int nPaths) {
    DiffusionGBM diffGBM(mu, sigma);
    MCEngine1D<DiffusionGBM, IRProvider<IRModeE::Const>, IRProvider<IRModeE::Const>,
        CcyE, CcyE> engine(20000, 20000);
    IRProvider<IRModeE::Const> providerConst(nullptr);
    auto t0 = time(nullptr);
    engine.Simulate<false>(t0, t0 + T * 24 * 60 * 60, tauMin, s0, nPaths, &diffGBM,
        &providerConst, &providerConst, CcyE::USD, CcyE::USD);
    auto res = engine.GetPaths();
    auto L1 = std::get<0>(res), P1 = std::get<1>(res);
    auto paths = std::get<2>(res);

    double EST = 0, EST2 = 0;
    long samples = 0;

    for (auto p = 0; p < P1; ++p) {
        if (*(paths + p * L1 + L1 - 1) <= 0) continue;
        auto RT = std::log(*(paths + p * L1 + L1 - 1) / *(paths + p * L1));
        EST += RT;
        EST2 += RT * RT;
        ++samples; 
    }

    assert(samples > 1);

    auto tYears = T / 365.25;

    auto sigma2Estimated = (EST2 - EST * EST / samples) / tYears / (samples - 1);

    auto muEstimated = EST / tYears / samples + sigma2Estimated / 2;

    printf("mu = %f, sigma^2 = %f\n", muEstimated, sigma2Estimated);
}

int main(int argc, char **argv) {
    if (argc != 7) {
        std::cerr << "usage: mu sigma s0 Tdays taumin p" << std::endl;
        return 1;
    }
    auto mu = atof(argv[1]), sigma = atof(argv[2]), s0 = atof(argv[3]);
    auto T = atol(argv[4]);
    int tauMin = atoi(argv[5]), nPaths = atoi(argv[6]);

    TestGBM(mu, sigma, s0, T, tauMin, nPaths);
    return 0;
}
