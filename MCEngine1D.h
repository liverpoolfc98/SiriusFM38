#pragma once
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <utility>
#include <tuple>

namespace siriusFM
{
    
template <class Diffusion1D,
          class AProvider,
          class BProvider,
          class AssetClassA,
          class AssetClassB,
          class PathEvaluator>
class MCEngine1D
{
public:
    MCEngine1D(long a_MaxL, long a_MaxP)
    :
    m_MaxL(a_MaxL),
    m_MaxP(a_MaxP),
    m_paths(new double[a_MaxL * a_MaxP]),
    m_ts(new double[a_MaxL]) {
        if (a_MaxL <= 0) {
            throw std::invalid_argument("non-positive arg a_MaxL in MCEngine1D constructor");
        }
        if (a_MaxP <= 0) {
            throw std::invalid_argument("non-positive arg a_MaxP in MCEngine1D constructor");
        }
        memset(m_paths, 0, sizeof(m_paths));
        memset(m_ts, 0, sizeof(m_ts));
    }

    template<bool IsRN>
    inline void Simulate(time_t a_t0,
                  time_t a_T,
                  int a_tau_min,
                  long a_P,
                  bool a_useTime,
                  const Diffusion1D* a_diff,
                  AProvider* a_rateA,
                  BProvider* a_rateB,
                  AssetClassA a_A,
                  AssetClassB a_B,
                  PathEvaluator* a_eval);

    MCEngine1D& operator=(const MCEngine1D&) = delete;

    MCEngine1D(const MCEngine1D&) = delete;

    ~MCEngine1D() {
        delete []m_paths;
        delete []m_ts;
    }
private:
    long const m_MaxL;          // Max PathLength
    long const m_MaxP;          // Max N Paths
    double* const m_paths;
    double* const m_ts;
};

}
