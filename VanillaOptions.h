#pragma once

#include "Option.h"

namespace siriusFM {

template<class AssetClassA, class AssetClassB>
class CallOption final: public Option<AssetClassA, AssetClassB> {
public:
    CallOption(double a_K, time_t a_expirTime, AssetClassA a_A, AssetClassB a_B,
    bool a_IsAmerican = false, bool a_IsAsian = false, bool a_IsEuropean = true)
    :
    Option<AssetClassA, AssetClassB>(a_expirTime, a_A, a_B, a_IsAmerican, a_IsAsian, a_IsEuropean),
    m_K(a_K) {
        if (m_K <= 0) {
            throw std::invalid_argument("K <= 0");
        }
    }
    double payoff(long a_L, const double* a_t, const double* a_s) const override {
        assert(a_L > 0 && a_t && a_s);
        return *(a_s + a_L - 1) > m_K ? *(a_s + a_L - 1) - m_K : 0;
    }
    ~CallOption() override {};
private:
    const double m_K;
};

template<class AssetClassA, class AssetClassB>
class PutOption final: public Option<AssetClassA, AssetClassB> {
public:
    PutOption(double a_K, time_t a_expirTime, AssetClassA a_A, AssetClassB a_B,
    bool a_IsAmerican = false, bool a_IsAsian = false, bool a_IsEuropean = true)
    :
    Option<AssetClassA, AssetClassB>(a_expirTime, a_A, a_B, a_IsAmerican, a_IsAsian, a_IsEuropean),
    m_K(a_K) {
        if (m_K <= 0) {
            throw std::invalid_argument("K <= 0");
        }
    }
    double payoff(long a_L, const double* a_t, const double* a_s) const override {
        assert(a_L > 0 && a_t && a_s);
        return *(a_s + a_L - 1) < m_K ? m_K - *(a_s + a_L - 1): 0;
    }
    ~PutOption() override {};
private:
    const double m_K;
};

using CallOptionFX = CallOption<CcyE, CcyE>;
using PutOptionFX = PutOption<CcyE, CcyE>;

}
