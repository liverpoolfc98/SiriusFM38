#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include "DiffusionGBM.h"
#include "IRProviderConst.h"
#include "MCEngine1D.hpp"
#include "VanillaOptions.h"
#include "MCOptionPricer.hpp"



using namespace siriusFM;

void TestOption(double mu, double sigma, double s0, Option* option, double K,
    long Tdays, int tauMin, long nPaths) {
    DiffusionGBM diffGBM(mu, sigma, s0);
    MCEngine1D<DiffusionGBM, IRProvider<IRModeE::Const>, IRProvider<IRModeE::Const>,
        CcyE, CcyE, OPPathEval> engine(20000, 20000);
    IRProvider<IRModeE::Const> providerConst(nullptr);
    OPPathEval eval(option);
    auto t0 = time(nullptr);
    engine.Simulate<true>(t0, t0 + Tdays * 24 * 60 * 60, tauMin, nPaths, false, &diffGBM,
        &providerConst, &providerConst, CcyE::USD, CcyE::USD, &eval);

    double price = eval.getPrice();

    printf("option price = %f\n", price);
    double err = price != 0 ? eval.getSTD() / price : eval.getSTD();
    printf("STD = %f\n", err);
    printf("Max = %f min = %f\n", eval.getMax(), eval.getMin());
}

int main(int argc, char** argv) {
    if (argc != 9) {
        std::cerr << "usage: mu sigma s0 put/call strike Tdays tauMin paths" << std::endl;
        return 1;
    }
    auto mu = atof(argv[1]), sigma = atof(argv[2]), s0 = atof(argv[3]);
    auto K = atol(argv[5]), Tdays = atol(argv[6]);
    Option* option;
    if (strcmp(argv[4], "Put") == 0) {
        option = new EurPutOption(K, Tdays);
    } else if (strcmp(argv[4], "Call") == 0) {
        option = new EurCallOption(K, Tdays);
    } else {
        throw std::invalid_argument("Put or Call required");
    }
    int tauMin = atoi(argv[7]), nPaths = atoi(argv[8]);

    TestOption(mu, sigma, s0, option, K, Tdays, tauMin, nPaths);

    delete option;

    return 0;
}
