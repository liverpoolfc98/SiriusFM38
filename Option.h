#pragma once

#include <stdexcept>
#include <ctime>

#include "IRProvider.h"

namespace siriusFM {

template<class AssetClassA, class AssetClassB>
class Option {
public:
    Option(time_t a_expirTime, bool a_IsAmerican, AssetClassA a_A, AssetClassB a_B, bool a_IsAsian = false)
    : m_expirTime(a_expirTime), m_IsAmerican(a_IsAmerican), m_IsAsian(a_IsAsian), m_A(a_A), m_B(a_B) {
        if (m_expirTime <= 0) {
            throw std::invalid_argument("Tdays <= 0");
        }
    }
    time_t expirTime() const { return m_expirTime; }
    bool IsAmerican() const { return m_IsAmerican; }
    bool IsAsian() const { return m_IsAsian; }
    AssetClassA A() const { return m_A; }
    AssetClassB B() const { return m_B; }
    virtual double payoff(long a_L, const double* a_t, const double* a_s) const = 0;
    virtual ~Option() {}
protected:
    const time_t m_expirTime;
    const bool m_IsAmerican;
    const bool m_IsAsian;
    AssetClassA m_A;
    AssetClassB m_B;
};

using OptionFX = Option<CcyE, CcyE>;

}
