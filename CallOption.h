#pragma once

namespace siriusFM {

class CallOption {
public:
    explicit CallOption(double a_strike, long int a_expiration_time)
    : m_strike(a_strike), m_expiration_time(a_expiration_time) {
        if (m_strike < 0) {
            throw std::invalid_argument("strike is negative");
        }
        if (m_expiration_time < 0) {
            throw std::invalid_argument("expiraion time is negative");
        }
    }
    double payoff(double a_s) const {
        return a_s > m_strike ? a_s - m_strike : 0;
    }
private:
    const double m_strike;
    const long int m_expiration_time;
};

}

