#pragma once

#include "IRProvider.h"


namespace siriusFM {

template <>
class IRProvider <IRModeE::Const> {
public:
    IRProvider(const char* a_file);
    double r(CcyE a_ccy, double a_t) {
        return m_IRs[static_cast<size_t>(a_ccy)];
    }
private:
    double m_IRs[static_cast<size_t>(CcyE::N)];
};

}
