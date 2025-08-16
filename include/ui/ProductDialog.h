#pragma once

#include <QDialog>
#include "domain/Product.h"

namespace quickqash::ui {

class ProductDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProductDialog(QWidget* parent = nullptr);
    ~ProductDialog();

    void setProduct(const quickqash::domain::Product& p);
    quickqash::domain::Product product() const;

private:
    // opaque pimpl not required yet
};

}
