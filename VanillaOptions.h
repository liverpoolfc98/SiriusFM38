#pragma once

#include "Option.h"

namespace siriusFM {

template<class AssetClassA, class AssetClassB>
class EurCallOption final: public Option<AssetClassA, AssetClassB> {
public:
    EurCallOption(double a_K, time_t a_expirTime, AssetClassA a_A, AssetClassB a_B)
    : Option<AssetClassA, AssetClassB>(a_expirTime, false, a_A, a_B), m_K(a_K) {
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

template<class AssetClassA, class AssetClassB>
class EurPutOption final: public Option<AssetClassA, AssetClassB> {
public:
    EurPutOption(double a_K, time_t a_expirTime, AssetClassA a_A, AssetClassB a_B)
    : Option<AssetClassA, AssetClassB>(a_expirTime, false, a_A, a_B), m_K(a_K) {
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

using EurCallOptionFX = EurCallOption<CcyE, CcyE>;
using EurPutOptionFX = EurPutOption<CcyE, CcyE>;

}
