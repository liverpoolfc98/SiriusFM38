#include <cmath>
#include <stdexcept>

#include "DiffusionCEV.h"
#include "DiffusionCIR.h"
#include "DiffusionGBM.h"
#include "DiffusionLipton.h"
#include "DiffusionOU.h"

int main() {
    siriusFM::DiffusionGBM a(1, 1);
    siriusFM::DiffusionOU b(1, 1, 1);
    siriusFM::DiffusionLipton c(1, 1, 1, 4);
    siriusFM::DiffusionCEV d(1, 1, 1);
    siriusFM::DiffusionCIR e(1, 1, 1);
    return 0;
}

