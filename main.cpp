#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <chrono>

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
    time_t t0 = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    engine.Simulate<false>(t0, t0 + T * 24 * 60 * 60, tauMin, s0, nPaths, &diffGBM,
        &providerConst, &providerConst, CcyE::USD, CcyE::USD);
    auto res = engine.GetPaths();
    long L1 = std::get<0>(res), P1 = std::get<1>(res);
    const double* paths = std::get<2>(res);

    double EST = 0, EST2 = 0;
    long samples = 0;

    for (long p = 0; p < P1; ++p) {
        if (paths[p * L1 + L1 - 1] <= 0) continue;
        double RT = std::log(paths[p * L1 + L1 - 1] / paths[p * L1]);
        EST += RT;
        EST2 += RT * RT;
        ++samples; 
    }

    assert(samples > 0);

    double tYears = static_cast<double>(T) / 365.25;

    double sigma2Estimated = 1. / (samples - 1) * (EST2 - EST * EST / samples) / tYears;

    double muEstimated = EST / samples / tYears + sigma2Estimated / 2;

    printf("mu = %f, sigma^2 = %f\n", muEstimated, sigma2Estimated);
}

int main(int argc, char **argv) {
    if (argc != 7) {
        std::cerr << "usage: mu sigma s0 Tdays taumin p" << std::endl;
        return 1;
    }
    double mu = atof(argv[1]), sigma = atof(argv[2]), s0 = atof(argv[3]);
    long T = atol(argv[4]);
    int tauMin = atoi(argv[5]), nPaths = atoi(argv[6]);

    TestGBM(mu, sigma, s0, T, tauMin, nPaths);
    return 0;
}
