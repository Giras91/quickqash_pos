#pragma once

#include <QWidget>

namespace quickqash::ui {

class ProductsView : public QWidget {
    Q_OBJECT
public:
    explicit ProductsView(QWidget* parent = nullptr);
    ~ProductsView();
};

}
