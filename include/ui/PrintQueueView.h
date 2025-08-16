#pragma once

#include <QWidget>

namespace quickqash::ui {

class PrintQueueView : public QWidget {
    Q_OBJECT
public:
    explicit PrintQueueView(QWidget* parent = nullptr);
    ~PrintQueueView();
};

}
