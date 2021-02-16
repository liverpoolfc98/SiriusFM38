#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include "DiffusionGBM.h"
#include "MCOptionHedger1D.hpp"
#include "GridNOP1D_S3_RKC1.hpp"
#include "Time.h"
#include "BSM.h"

using namespace siriusFM;

int main(int argc, char** argv) {
    if (argc != 10) {
        std::cerr << "usage: sigma s0 put/call strike Tdays IsAmerican RatesFile NS tauMin" << std::endl;
        return 1;
    }

    auto t0 = time(nullptr);

    auto sigma = atof(argv[1]);
    auto s0 = atof(argv[2]);
    auto optType = argv[3];
    auto K = atol(argv[4]);
    auto Tdays = atol(argv[5]);
    auto IsAmerican = static_cast<bool>(argv[6]);
    auto ratesFile = argv[7];
    auto NS = atol(argv[8]);
    auto tauMin = atoi(argv[9]);

    OptionFX* option;

    // American Asian European

    if (strcmp(argv[3], "Put") == 0) {
        option = new PutOptionFX(K, time(nullptr) + Tdays * 24 * 60 * 60,
            CcyE::USD, CcyE::RUB, IsAmerican, false, true);
    } else if (strcmp(argv[3], "Call") == 0) {
        option = new CallOptionFX(K, time(nullptr) + Tdays * 24 * 60 * 60,
            CcyE::USD, CcyE::RUB, IsAmerican, false, true);
    } else {
        throw std::invalid_argument("Put or Call required");
    }

    DiffusionGBM diffGBM(0, sigma, s0);

    GridNOP1D<DiffusionGBM, IRProvider<IRModeE::Const>, IRProvider<IRModeE::Const>,
        CcyE, CcyE> grid(ratesFile, ratesFile, 130000, 3000);


    grid.Run<false>(option, t0, &diffGBM, s0, NS, tauMin);

    auto res = grid.getStats();

    printf("price = %f\ngamma = %f\ndelta = %f\n",
        std::get<0>(res), std::get<1>(res), std::get<2>(res));

    delete option;

    return 0;
}
