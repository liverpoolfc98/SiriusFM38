#pragma once

namespace siriusFM {

class DiffusionGBM {
public:
    explicit DiffusionGBM(double a_mu, double a_sigma) : m_mu(a_mu), m_sigma(a_sigma) {
        if (m_sigma <= 0) {
            throw std::invalid_argument("sigma is non-positive");
        }
    }
    double mu(double a_s, double a_t) const { return m_mu * a_s; }
    double sigma(double a_s, double a_t) const { return a_s < 0 ? 0 : m_sigma * a_s; }
private:
    const double m_mu, m_sigma;
};

}

