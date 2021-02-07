#include <cmath>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

#include "IRProviderConst.h"
#include "IRProvider.h"
#include "DiffusionGBM.h"
#include "DiffusionCEV.h"
#include "DiffusionCIR.h"
#include "DiffusionLipton.h"
#include "DiffusionOU.h"

void Test() {
    siriusFM::DiffusionGBM diffGBM(1, 1);
    siriusFM::DiffusionCEV diffCEV(1, 1, 1);
    siriusFM::DiffusionCIR diffCIR(1, 1, 1);
    siriusFM::DiffusionLipton diffLipton(1, 1, 1, 5);
    siriusFM::DiffusionOU diffOU(1, 1, 1);
    siriusFM::IRProvider<siriusFM::IRModeE::Const> providerConst("test.txt");
}

int main() {
    Test();
    return 0;
}
