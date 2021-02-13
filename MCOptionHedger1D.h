#pragma once

#include <functional>

#include "MCEngine1D.hpp"
#include "VanillaOptions.h"

namespace siriusFM {

template <class Diffusion1D,
          class AProvider,
          class BProvider,
          class AssetClassA,
          class AssetClassB>
class MCOptionHedger1D {
public:
    using DeltaFunc = std::function<double(double, double)>; // (S, t) -> delta
    MCOptionHedger1D(
        const char*         a_irs_A,
        const char*         a_irs_B,
        const Diffusion1D*  a_diff,
        bool                a_useTime
    )
    :
    m_irpA(a_irs_A),
    m_irpB(a_irs_B),
    m_diff(a_diff),
    m_useTime(a_useTime),
    m_mce(102271, 4096) {}

    std::tuple<double, double, double, double> SimulateHedging(
        Option<AssetClassA, AssetClassB>* a_option,
        time_t                            a_t0,
        int                               a_tauMins,
        long                              a_P,
        double                            a_C0,
        DeltaFunc const*                  a_DeltaFunc,
        double                            a_DeltaAcc
    );

    double getRateA(AssetClassA a_assetA, double a_t) const {
        return m_irpA.r(a_assetA, a_t);
    }

    double getRateB(AssetClassB a_assetB, double a_t) const {
        return m_irpB.r(a_assetB, a_t);
    }

private:

    class OHPathEval {
    public:
        
        OHPathEval(
            Option<AssetClassA, AssetClassB>* a_option,
            AProvider*                        a_irpA,
            BProvider*                        a_irpB,
            double                            a_DeltaAcc,
            double                            a_C0,
            const DeltaFunc* const            a_DeltaFunc
        )
        :
        m_P(0),
        m_C0(a_C0),
        m_option(a_option),
        m_irpA(a_irpA),
        m_irpB(a_irpB),
        m_ratesA(nullptr),
        m_ratesB(nullptr),
        m_sumPnL(0),
        m_sumPnL2(0),
        m_minPnL(INFINITY),
        m_maxPnL(-INFINITY),
        m_DeltaAcc(a_DeltaAcc),
        m_DeltaFunc(a_DeltaFunc) {
            assert(m_option && m_irpA && m_irpB && m_DeltaFunc && m_DeltaAcc >= 0);
        }

        void operator()(long a_L, long a_PM, const double* a_paths, const double* a_ts) {

            if (!m_ratesA) {
                m_ratesA = new double[a_L];
                for (auto l = 0; l < a_L; ++l) {
                    m_ratesA[l] = m_irpA->r(m_option->A(), a_ts[l]);
                }
            }

            if (!m_ratesB) {
                m_ratesB = new double[a_L];
                for (auto l = 0; l < a_L; ++l) {
                    m_ratesB[l] = m_irpB->r(m_option->B(), a_ts[l]);
                }
            }

            for (auto p = 0; p < a_PM; ++p) {

                auto path = a_paths + p * a_L;

                // perform delta-hedging along this path

                auto M = -m_C0;
                double delta = 0; // Curr delta

                for (auto l = 0; l < a_L; ++l) {
                    auto St = path[l]; // Curr underlying px
                    auto t = a_ts[l];  // Curr time

                    if (l > 0) {
                        auto tau = t - a_ts[l - 1];
                        M += M * tau * m_ratesB[l - 1];
                        M += path[l - 1] * tau * m_ratesA[l - 1]; // dividends
                    }

                    if (l < a_L - 1) {
                        auto deltaN = (*m_DeltaFunc)(St, t);
                        deltaN = -round(deltaN / m_DeltaAcc) * m_DeltaAcc;
                        if (delta != deltaN) {
                            M += (delta - deltaN) * St;
                            delta = deltaN;
                        }
                    }
                }

                M += m_option->payoff(a_L, a_ts, path);

                double PnL = M + delta * path[a_L - 1];

                m_sumPnL += PnL;
                m_sumPnL2 += PnL * PnL;
                m_minPnL = m_minPnL > PnL ? PnL : m_minPnL;
                m_maxPnL = m_maxPnL < PnL ? PnL : m_maxPnL;
            }
            m_P += a_PM;
        }

        ~OHPathEval() {
            delete []m_ratesA;
            delete []m_ratesB;
            m_ratesA = m_ratesB = nullptr;
        }

        double getEPnL() const {
            return m_sumPnL / m_P;
        }

        double getSTDPnL() const {
            return std::sqrt((m_sumPnL2 - m_sumPnL * m_sumPnL / m_P) / (m_P - 1));
        }

        double getMin() const {
            return m_minPnL;
        }

        double getMax() const {
            return m_maxPnL;
        }

        std::tuple<double, double, double, double> getStats() const {
            return std::make_tuple(getEPnL(), getSTDPnL(), getMin(), getMax());
        }

    private:
        const Option<AssetClassA, AssetClassB>* m_option;
        const AProvider* m_irpA;
        const BProvider* m_irpB;
        double* m_ratesA;
        double* m_ratesB;
        double m_sumPnL, m_sumPnL2, m_minPnL, m_maxPnL; // statistics of residual PnLs
        const double m_DeltaAcc;
        const double m_C0; // initial option premium
        long m_P;
        const DeltaFunc* const m_DeltaFunc;
    };

    bool m_useTime;
    AProvider m_irpA;
    BProvider m_irpB;
    const Diffusion1D* m_diff;
    MCEngine1D
    <Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB, OHPathEval> m_mce;
};

}
