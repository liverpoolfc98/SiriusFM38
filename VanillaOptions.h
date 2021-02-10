#pragma once
#include "Option.h"

namespace siriusFM {

class EurCallOption final: public Option {
public:
    EurCallOption(double a_K, double a_Tdays)
    : Option(a_Tdays, false), m_K(a_K) {
        if (m_K <= 0) {
            throw std::invalid_argument("K <= 0");
        }
    }
    double payoff(long a_L, const double* a_t, const double* a_s) const override {
        assert(a_L > 0 && a_t && a_s);
        return *(a_s + a_L - 1) > m_K ? *(a_s + a_L - 1) - m_K : 0;
    }
    ~EurCallOption() override {};
private:
    const double m_K;
};

class EurPutOption final: public Option {
public:
    EurPutOption(double a_K, double a_Tdays)
    : Option(a_Tdays, false), m_K(a_K) {
        if (m_K <= 0) {
            throw std::invalid_argument("K <= 0");
        }
    }
    double payoff(long a_L, const double* a_t, const double* a_s) const override {
        assert(a_L > 0 && a_t && a_s);
        return *(a_s + a_L - 1) < m_K ? m_K - *(a_s + a_L - 1): 0;
    }
    ~EurPutOption() override {};
private:
    const double m_K;
};

}
