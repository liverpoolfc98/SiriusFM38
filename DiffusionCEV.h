#pragma once

namespace siriusFM {

class DiffusionCEV {
public:
    explicit DiffusionCEV(double a_mu, double a_sigma, double a_beta)
    : m_mu(a_mu), m_sigma(a_sigma), m_beta(a_beta) {
        if (m_sigma <= 0) {
            throw std::invalid_argument("sigma is non-positive");
        }
        if (m_beta < 0) {
            throw std::invalid_argument("beta is negative");
        }
    }
    double mu(double a_s, double a_t) const { return m_mu * a_s; }
    double sigma(double a_s, double a_t) const {
        return a_s < 0 ? 0 : m_sigma * std::pow(a_s, m_beta);
    }
private:
    const double m_mu, m_sigma, m_beta;
};

}