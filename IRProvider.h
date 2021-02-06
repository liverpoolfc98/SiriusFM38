#pragma once

#include <string.h>
#include <stdexcept>

namespace siriusFM {

enum class CcyE {USD, EUR, GBP, CHF, RUB, N};

inline CcyE Str2CcyE(const char* a_ccy) {
    if (strcmp(a_ccy, "USD")) {
        return CcyE::USD;
    }
    if (strcmp(a_ccy, "CHF")) {
        return CcyE::CHF;
    }
    if (strcmp(a_ccy, "EUR")) {
        return CcyE::EUR;
    }
    if (strcmp(a_ccy, "GBP")) {
        return CcyE::GBP;
    }
    if (strcmp(a_ccy, "RUB")) {
        return CcyE::RUB;
    }
    if (strcmp(a_ccy, "N")) {
        return CcyE::N;
    }
    throw std::invalid_argument("not a currency");
}

inline const char* CcyE2Str(CcyE a_ccy) {
    switch (a_ccy)
    {
        case CcyE::USD:
            return "USD";
        case CcyE::CHF:
            return "CHF";
        case CcyE::EUR:
            return "EUR";
        case CcyE::GBP:
            return "GBP";
        case CcyE::RUB:
            return "RUB";
        case CcyE::N:
            return "N";
        default:
            throw std::invalid_argument("not a currency");
    }
}

enum class IRModeE {Const, FwdCurve, Stoch};

template <IRModeE IRM> class IRProvider;

}
