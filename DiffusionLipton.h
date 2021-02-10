#pragma once

namespace siriusFM {

class DiffusionLipton {
public:
    explicit DiffusionLipton(double a_mu, double a_sigma0, double a_sigma1, double a_sigma2, double a_s0)
    : m_mu(a_mu), m_sigma0(a_sigma0), m_sigma1(a_sigma1), m_sigma2(a_sigma2), m_s0(a_s0) {
        if (m_sigma1 * m_sigma1 - 4 * m_sigma0 * m_sigma2 >= 0 || m_sigma2 < 0) {
            throw std::invalid_argument("sigma is non-positive");
        }
    }
    double mu(double a_s, double a_t) const { return m_mu * a_s; }
    double sigma(double a_s, double a_t) const {
        return a_s < 0 ? 0 : m_sigma0 + m_sigma1 * a_s + m_sigma2 * a_s * a_s;
    }
    double s0() const { return m_s0; }
private:
    const double m_mu, m_sigma0, m_sigma1, m_sigma2, m_s0;
};

}

