#include <cmath>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

#include "IRProviderConst.h"

int main() {
    siriusFM::IRProvider<siriusFM::IRModeE::Const> a("test.txt");
    return 0;
}
