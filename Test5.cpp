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
    if (argc != 8) {
        std::cerr << "usage: sigma s0 put/call strike Tdays NS tauMin" << std::endl;
        return 1;
    }

    auto t0 = time(nullptr);

    auto sigma = atof(argv[1]);
    auto s0 = atof(argv[2]);
    auto optType = argv[3];
    auto K = atol(argv[4]);
    auto Tdays = atol(argv[5]);
    auto NS = atol(argv[6]);
    auto tauMin = atoi(argv[7]);

    OptionFX* option;

    if (strcmp(argv[3], "Put") == 0) {
        option = new EurPutOptionFX(K, time(nullptr) + Tdays * 24 * 60 * 60, CcyE::USD, CcyE::USD);
    } else if (strcmp(argv[3], "Call") == 0) {
        option = new EurCallOptionFX(K, time(nullptr) + Tdays * 24 * 60 * 60, CcyE::USD, CcyE::USD);
    } else {
        throw std::invalid_argument("Put or Call required");
    }

    DiffusionGBM diffGBM(0, sigma, s0);

    GridNOP1D<DiffusionGBM, IRProvider<IRModeE::Const>, IRProvider<IRModeE::Const>,
        CcyE, CcyE> grid(nullptr, nullptr, 5000, 10384);


    grid.RunBI(option, t0, &diffGBM, s0);

    delete option;

    return 0;
}
