#include "ui/ProductDialog.h"
#include <QtUiTools/QUiLoader>
#include <QFile>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

using namespace quickqash::ui;
using quickqash::domain::Product;

class ProductDialogPrivate {
public:
    QWidget* root = nullptr;
    QLineEdit* edit_barcode = nullptr;
    QLineEdit* edit_name = nullptr;
    QLineEdit* edit_price = nullptr;
    QSpinBox* spin_stock = nullptr;
    QPushButton* btn_save = nullptr;
    QPushButton* btn_cancel = nullptr;
};

ProductDialog::ProductDialog(QWidget* parent) : QDialog(parent) {
    QUiLoader loader;
    QFile f("ui/ProductDialog.ui");
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open ui/ProductDialog.ui";
        return;
    }
    QWidget* root = loader.load(&f, this);
    f.close();
    if (!root) {
        qWarning() << "Failed to load ProductDialog UI";
        return;
    }

    auto d = new ProductDialogPrivate();
    d->root = root;
    d->edit_barcode = root->findChild<QLineEdit*>("edit_barcode");
    d->edit_name = root->findChild<QLineEdit*>("edit_name");
    d->edit_price = root->findChild<QLineEdit*>("edit_price");
    d->spin_stock = root->findChild<QSpinBox*>("spin_stock");
    d->btn_save = root->findChild<QPushButton*>("btn_save");
    d->btn_cancel = root->findChild<QPushButton*>("btn_cancel");

    // adopt layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(root);

    if (d->btn_cancel) connect(d->btn_cancel, &QPushButton::clicked, this, &QDialog::reject);
    if (d->btn_save) connect(d->btn_save, &QPushButton::clicked, [this, d]() {
        // simple validation
        if (d->edit_barcode->text().trimmed().isEmpty()) {
            qWarning() << "Barcode required";
            return;
        }
        this->accept();
    });

    this->setModal(true);
    this->setMinimumSize(420, 320);

    // store private in QObject for lifetime management
    this->setProperty("_pd", QVariant::fromValue((void*)d));
}

ProductDialog::~ProductDialog() {
    void* vp = this->property("_pd").value<void*>();
    if (vp) delete static_cast<ProductDialogPrivate*>(vp);
}

void ProductDialog::setProduct(const Product& p) {
    void* vp = this->property("_pd").value<void*>();
    if (!vp) return;
    auto d = static_cast<ProductDialogPrivate*>(vp);
    if (d->edit_barcode) d->edit_barcode->setText(p.barcode);
    if (d->edit_name) d->edit_name->setText(p.name);
    if (d->edit_price) d->edit_price->setText(QString::number(p.price_cents / 100.0, 'f', 2));
    if (d->spin_stock) d->spin_stock->setValue(p.stock);
}

Product ProductDialog::product() const {
    Product p;
    void* vp = this->property("_pd").value<void*>();
    if (!vp) return p;
    auto d = static_cast<ProductDialogPrivate*>(vp);
    if (d->edit_barcode) p.barcode = d->edit_barcode->text();
    if (d->edit_name) p.name = d->edit_name->text();
    if (d->edit_price) p.price_cents = static_cast<qint64>(std::llround(d->edit_price->text().toDouble() * 100.0));
    if (d->spin_stock) p.stock = d->spin_stock->value();
    return p;
}
