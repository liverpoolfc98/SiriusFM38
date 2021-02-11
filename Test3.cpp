#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include "DiffusionGBM.h"
#include "MCOptionPricer1D.hpp"

using namespace siriusFM;

void Test(double mu, double sigma, double s0, bool useTime, OptionFX* option,
    int tauMins, long nPaths) {
    DiffusionGBM diffGBM(mu, sigma, s0);
    MCOptionPricer1D<DiffusionGBM, IRProvider<IRModeE::Const>, IRProvider<IRModeE::Const>,
    CcyE, CcyE> pricer(nullptr, nullptr, &diffGBM, useTime);
    auto t0 = time(nullptr);
    auto stats = pricer.price(option, t0, tauMins, nPaths);

    printf("option price = %f\n", std::get<0>(stats));
    printf("error = %f\n", std::get<1>(stats));
}

int main(int argc, char** argv) {
    if (argc != 10) {
        std::cerr << "usage: mu sigma s0 put/call strike Tdays tauMin paths" << std::endl;
        return 1;
    }
    auto mu = atof(argv[1]), sigma = atof(argv[2]), s0 = atof(argv[3]);
    auto K = atol(argv[5]), Tdays = atol(argv[6]);
    bool useTime = bool(atoi(argv[9]));
    OptionFX* option;
    if (strcmp(argv[4], "Put") == 0) {
        option = new EurPutOptionFX(K, time(nullptr) + Tdays * 24 * 60 * 60, CcyE::USD, CcyE::USD);
    } else if (strcmp(argv[4], "Call") == 0) {
        option = new EurCallOptionFX(K, time(nullptr) + Tdays * 24 * 60 * 60, CcyE::USD, CcyE::USD);
    } else {
        throw std::invalid_argument("Put or Call required");
    }
    int tauMin = atoi(argv[7]), nPaths = atoi(argv[8]);

    Test(mu, sigma, s0, useTime, option, tauMin, nPaths);

    delete option;

    return 0;
}
