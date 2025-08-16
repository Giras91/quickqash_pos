#pragma once

#include <QString>
#include <QVariant>

namespace quickqash::domain {

struct Product {
    int id = 0;
    QString barcode;
    QString name;
    qint64 price_cents = 0; // stored as cents
    int stock = 0;
};

}
