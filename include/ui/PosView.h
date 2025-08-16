#pragma once

#include <QWidget>

namespace quickqash::ui {

class PosView : public QWidget {
    Q_OBJECT
public:
    explicit PosView(QWidget* parent = nullptr);
    ~PosView();
};

}
