#pragma once

#include "IRProvider.h"
#include "Time.h"


namespace siriusFM {

template <>
class IRProvider <IRModeE::Const> {
public:
    IRProvider(const char* a_file);
    double r(CcyE a_ccy, double a_t) const {
        return m_IRs[static_cast<size_t>(a_ccy)];
    }
    double DT(CcyE a_ccy, time_t a_t0, time_t a_t1) const {
        return std::exp(-(YearFrac(a_t1) - YearFrac(a_t0)) * m_IRs[static_cast<size_t>(a_ccy)]);
    }
private:
    double m_IRs[static_cast<size_t>(CcyE::N)];
};

}
