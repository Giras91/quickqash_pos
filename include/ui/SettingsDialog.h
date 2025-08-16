#pragma once

#include <QDialog>

namespace quickqash::ui {

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();
};

}
