#pragma once

#include <stdexcept>

namespace siriusFM {

class Option {
public:
    Option(double a_Tdays, bool a_IsAmerican)
    : m_Tdays(a_Tdays), m_IsAmerican(a_IsAmerican) {
        if (m_Tdays <= 0) {
            throw std::invalid_argument("Tdays <= 0");
        }
    }
    bool IsAmerican() const {return m_IsAmerican;}
    virtual double payoff(long a_L, const double* a_t, const double* a_s) const = 0;
    virtual ~Option() {}
protected:
    const bool m_IsAmerican;
    const double m_Tdays;
};

}
