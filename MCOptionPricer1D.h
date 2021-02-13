#pragma once

#include "MCEngine1D.hpp"
#include "Option.h"

namespace siriusFM {

template <class Diffusion1D,
          class AProvider,
          class BProvider,
          class AssetClassA,
          class AssetClassB>
class MCOptionPricer1D {
public:
    MCOptionPricer1D(
        const char* a_irs_A,
        const char* a_irs_B,
        const Diffusion1D* a_diff,
        bool a_useTime
    )
    :
    m_irpA(a_irs_A),
    m_irpB(a_irs_B),
    m_diff(a_diff),
    m_useTime(a_useTime),
    m_mce(102271, 4096) {}

    std::pair<double, double> price(
        Option<AssetClassA, AssetClassB>* a_option,
        time_t a_t0,
        int a_tauMins,
        long a_P
    );

private:

    class OPPathEval {
    public:
        OPPathEval(Option<AssetClassA, AssetClassB>* a_option)
        : m_min(HUGE_VAL), m_max(0), m_sum(0), m_sum2(0), m_P(0), m_option(a_option) {}
        void operator()(long a_L, long a_PM, const double* a_paths, const double* a_ts) {
            for (auto p = 0; p < a_PM; ++p) {
                auto st = m_option->payoff(a_L, a_ts, a_paths + p * a_L);
                m_sum += st;
                m_sum2 += st * st;
                m_max = m_max < st ? st : m_max;
                m_min = m_min > st ? st : m_min;
            }
            m_P += a_PM;
        }
        double getPrice() const { return m_sum / m_P; }
        double getSTD() const {
            return std::sqrt((m_sum2 - m_sum * m_sum / m_P) / (m_P - 1));
        }
        double getMin() const { return m_min; }
        double getMax() const { return m_max; }
        std::pair<double, double> getPriceStats() const { return {getPrice(), getSTD()}; }
    private:
        const Option<AssetClassA, AssetClassB>* m_option;
        double m_min, m_sum, m_sum2, m_max;
        long m_P;
    };

    bool m_useTime;
    AProvider m_irpA;
    BProvider m_irpB;
    const Diffusion1D* m_diff;
    MCEngine1D
    <Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB, OPPathEval> m_mce;
};

}
