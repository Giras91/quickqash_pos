#include "util/Money.h"
#include <cmath>
#include <QString>

using namespace quickqash::util;

QString Money::toString() const {
    double v = toDouble();
    return QString::number(v, 'f', 2);
}
