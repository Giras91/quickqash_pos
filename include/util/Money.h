#pragma once

#include <QString>

namespace quickqash::util {

class Money {
public:
    explicit Money(qint64 cents = 0) : m_cents(cents) {}
    static Money fromDouble(double value) { return Money(static_cast<qint64>(std::llround(value * 100.0))); }
    double toDouble() const { return m_cents / 100.0; }
    qint64 cents() const { return m_cents; }
    QString toString() const;
private:
    qint64 m_cents;
};

}
