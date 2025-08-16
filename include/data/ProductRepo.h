#pragma once

#include <QString>
#include <optional>
#include "domain/Product.h"

namespace quickqash::data {

class ProductRepo {
public:
    ProductRepo();
    std::optional<quickqash::domain::Product> byBarcode(const QString& barcode);
    bool save(const quickqash::domain::Product& p);
    bool deleteById(int id);
};

}
