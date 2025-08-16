#include "ui/SettingsDialog.h"
#include <QtUiTools/QUiLoader>
#include <QFile>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include "util/Settings.h"

using namespace quickqash::ui;
using quickqash::util::Settings;

class SettingsDialogPrivate {
public:
    QWidget* root = nullptr;
    QDoubleSpinBox* spin_tax = nullptr;
    QLineEdit* edit_printer_host = nullptr;
    QSpinBox* spin_printer_port = nullptr;
    QPushButton* btn_save = nullptr;
    QPushButton* btn_cancel = nullptr;
};

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    QUiLoader loader;
    QFile f("ui/SettingsDialog.ui");
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open SettingsDialog UI";
        return;
    }
    QWidget* root = loader.load(&f, this);
    f.close();
    if (!root) {
        qWarning() << "Failed to load SettingsDialog UI";
        return;
    }

    auto d = new SettingsDialogPrivate();
    d->root = root;
    d->spin_tax = root->findChild<QDoubleSpinBox*>("spin_tax");
    d->edit_printer_host = root->findChild<QLineEdit*>("edit_printer_host");
    d->spin_printer_port = root->findChild<QSpinBox*>("spin_printer_port");
    d->btn_save = root->findChild<QPushButton*>("btn_save");
    d->btn_cancel = root->findChild<QPushButton*>("btn_cancel");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(root);

    // populate with current settings
    Settings::instance().load();
    if (d->spin_tax) d->spin_tax->setValue(Settings::instance().taxRate() * 100.0);
    if (d->edit_printer_host) d->edit_printer_host->setText(Settings::instance().printerHost());
    if (d->spin_printer_port) d->spin_printer_port->setValue(Settings::instance().printerPort());

    if (d->btn_cancel) connect(d->btn_cancel, &QPushButton::clicked, this, &QDialog::reject);
    if (d->btn_save) connect(d->btn_save, &QPushButton::clicked, [this, d]() {
        // save settings
        double taxPct = d->spin_tax ? d->spin_tax->value() : 0.0;
        Settings::instance().setTaxRate(taxPct / 100.0);
        if (d->edit_printer_host) Settings::instance().setPrinterHost(d->edit_printer_host->text());
        if (d->spin_printer_port) Settings::instance().setPrinterPort(d->spin_printer_port->value());
        Settings::instance().save();
        this->accept();
    });

    this->setModal(true);
    this->setMinimumSize(380, 200);
    this->setProperty("_pd", QVariant::fromValue((void*)d));
}

SettingsDialog::~SettingsDialog() {
    void* vp = this->property("_pd").value<void*>();
    if (vp) delete static_cast<SettingsDialogPrivate*>(vp);
}
