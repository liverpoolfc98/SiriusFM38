#pragma once

namespace siriusFM {

class DiffusionOU {
public:
    explicit DiffusionOU(double a_theta, double a_kapa, double a_sigma, double a_s0)
    : m_theta(a_theta), m_kapa(a_kapa), m_sigma(a_sigma), m_s0(a_s0) {
        if (m_sigma <= 0) {
            throw std::invalid_argument("sigma is non-positive");
        }
        if (m_kapa <= 0) {
            throw std::invalid_argument("kapa is non-positive");
        }
    }
    double mu(double a_s, double a_t) const { return m_kapa * (m_theta - a_s); }
    double sigma(double a_s, double a_t) const { return a_s < 0 ? 0 : m_sigma; }
    double s0() const { return m_s0; }
private:
    const double m_theta, m_kapa, m_sigma, m_s0;
};

}

