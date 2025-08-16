#include "ui/PosView.h"
#include <QtUiTools/QUiLoader>
#include <QFile>
#include <QLineEdit>
#include <QTableView>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include "data/ProductRepo.h"
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

using namespace quickqash::ui;
using quickqash::data::ProductRepo;
using quickqash::domain::Product;

class PosViewPrivate {
public:
    QWidget* root = nullptr;
    QLineEdit* edit_barcode = nullptr;
    QTableView* table = nullptr;
    QStandardItemModel* model = nullptr;
    QLabel* lbl_subtotal = nullptr;
    QLabel* lbl_tax = nullptr;
    QLabel* lbl_discount = nullptr;
    QLabel* lbl_total = nullptr;
    QPushButton* btn_checkout = nullptr;
};

PosView::PosView(QWidget* parent) : QWidget(parent) {
    QUiLoader loader;
    QFile f("ui/PosView.ui");
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open ui/PosView.ui";
        return;
    }
    QWidget* root = loader.load(&f, this);
    f.close();
    if (!root) {
        qWarning() << "Failed to load PosView UI";
        return;
    }

    auto d = new PosViewPrivate();
    d->root = root;
    d->edit_barcode = root->findChild<QLineEdit*>("edit_barcode");
    d->table = root->findChild<QTableView*>("table_cart");

    d->model = new QStandardItemModel(0, 4, this);
    d->model->setHeaderData(0, Qt::Horizontal, "Barcode");
    d->model->setHeaderData(1, Qt::Horizontal, "Name");
    d->model->setHeaderData(2, Qt::Horizontal, "Qty");
    d->model->setHeaderData(3, Qt::Horizontal, "Price");

    if (d->table) {
        d->table->setModel(d->model);
        d->table->horizontalHeader()->setStretchLastSection(true);
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(root);

    // barcode handling
    if (d->edit_barcode) {
        connect(d->edit_barcode, &QLineEdit::returnPressed, [this, d]() {
            QString code = d->edit_barcode->text().trimmed();
            if (code.isEmpty()) return;
            ProductRepo repo;
            auto opt = repo.byBarcode(code);
            if (opt) {
                Product p = *opt;
                // append to cart (simple append)
                int row = d->model->rowCount();
                d->model->insertRow(row);
                d->model->setData(d->model->index(row, 0), p.barcode);
                d->model->setData(d->model->index(row, 1), p.name);
                d->model->setData(d->model->index(row, 2), 1);
                d->model->setData(d->model->index(row, 3), QString::number(p.price_cents / 100.0, 'f', 2));
                d->edit_barcode->clear();
                // update totals
                // simple recompute
                double subtotal = 0.0;
                for (int r = 0; r < d->model->rowCount(); ++r) {
                    double price = d->model->data(d->model->index(r, 3)).toDouble();
                    int qty = d->model->data(d->model->index(r, 2)).toInt();
                    subtotal += price * qty;
                }
                double tax = subtotal * 0.06; // hard-coded 6% tax for demo
                double discount = 0.0;
                double total = subtotal + tax - discount;
                if (d->lbl_subtotal) d->lbl_subtotal->setText(QString("Subtotal: %1").arg(QString::number(subtotal, 'f', 2)));
                if (d->lbl_tax) d->lbl_tax->setText(QString("Tax: %1").arg(QString::number(tax, 'f', 2)));
                if (d->lbl_discount) d->lbl_discount->setText(QString("Discount: %1").arg(QString::number(discount, 'f', 2)));
                if (d->lbl_total) d->lbl_total->setText(QString("Total: %1").arg(QString::number(total, 'f', 2)));
            } else {
                qWarning() << "Product not found for barcode" << code;
            }
        });
    }

    d->lbl_subtotal = root->findChild<QLabel*>("label_subtotal");
    d->lbl_tax = root->findChild<QLabel*>("label_tax");
    d->lbl_discount = root->findChild<QLabel*>("label_discount");
    d->lbl_total = root->findChild<QLabel*>("label_total");
    d->btn_checkout = root->findChild<QPushButton*>("btn_checkout");

    if (d->btn_checkout) {
        connect(d->btn_checkout, &QPushButton::clicked, [this, d]() {
            // build sale data
            double subtotal = 0.0;
            for (int r = 0; r < d->model->rowCount(); ++r) {
                double price = d->model->data(d->model->index(r, 3)).toDouble();
                int qty = d->model->data(d->model->index(r, 2)).toInt();
                subtotal += price * qty;
            }
            qint64 subtotal_cents = static_cast<qint64>(std::llround(subtotal * 100.0));
            double taxRate = quickqash::util::Settings::instance().taxRate();
            qint64 tax_cents = static_cast<qint64>(std::llround(subtotal * taxRate * 100.0));
            qint64 total_cents = subtotal_cents + tax_cents;

            QSqlDatabase db = quickqash::data::Db::instance().database();
            if (!db.isOpen()) {
                QMessageBox::critical(this, "Checkout failed", "Database is not open.");
                return;
            }

            QSqlQuery q(db);
            if (!q.exec("BEGIN IMMEDIATE;")) {
                QMessageBox::critical(this, "Checkout failed", "Unable to begin transaction: " + q.lastError().text());
                return;
            }

            q.prepare("INSERT INTO sales (total_cents, paid_cents, cashier) VALUES (:total, :paid, :cashier)");
            q.bindValue(":total", total_cents);
            q.bindValue(":paid", total_cents);
            q.bindValue(":cashier", QString("demo"));
            if (!q.exec()) {
                db.rollback();
                QMessageBox::critical(this, "Checkout failed", "Failed to insert sale: " + q.lastError().text());
                return;
            }
            qint64 saleId = q.lastInsertId().toLongLong();

            // insert sale items and decrement stock
            bool ok = true;
            for (int r = 0; r < d->model->rowCount(); ++r) {
                QString barcode = d->model->data(d->model->index(r, 0)).toString();
                int qty = d->model->data(d->model->index(r, 2)).toInt();
                double price = d->model->data(d->model->index(r, 3)).toDouble();
                qint64 price_cents = static_cast<qint64>(std::llround(price * 100.0));
                // ensure stock is sufficient and decrement
                QSqlQuery qcheck(db);
                qcheck.prepare("SELECT id, stock FROM products WHERE barcode = :barcode");
                qcheck.bindValue(":barcode", barcode);
                if (!qcheck.exec() || !qcheck.next()) {
                    ok = false;
                    qWarning() << "Product not found during checkout:" << barcode;
                    break;
                }
                int productId = qcheck.value(0).toInt();
                int stock = qcheck.value(1).toInt();
                if (stock < qty) {
                    ok = false;
                    qWarning() << "Insufficient stock for" << barcode;
                    break;
                }
                QSqlQuery qi(db);
                qi.prepare("INSERT INTO sale_items (sale_id, product_id, qty, price_cents) VALUES (:sale, :pid, :qty, :price)");
                qi.bindValue(":sale", saleId);
                qi.bindValue(":pid", productId);
                qi.bindValue(":qty", qty);
                qi.bindValue(":price", price_cents);
                if (!qi.exec()) {
                    ok = false;
                    qWarning() << "Insert sale_item failed:" << qi.lastError().text();
                    break;
                }
                QSqlQuery qdec(db);
                qdec.prepare("UPDATE products SET stock = stock - :qty WHERE id = :id AND stock >= :qty");
                qdec.bindValue(":qty", qty);
                qdec.bindValue(":id", productId);
                if (!qdec.exec() || qdec.numRowsAffected() == 0) {
                    ok = false;
                    qWarning() << "Failed to decrement stock for product id" << productId;
                    break;
                }
            }

            if (!ok) {
                db.rollback();
                QMessageBox::critical(this, "Checkout failed", "Failed to insert sale items.");
                return;
            }

            if (!db.commit()) {
                QMessageBox::critical(this, "Checkout failed", "Failed to commit transaction.");
                return;
            }
            QMessageBox::information(this, "Checkout", "Sale recorded successfully.");
            // Build receipt bytes and send to printer
            QByteArray receipt;
            receipt.append(quickqash::printing::EscPos::text("QuickQash\n"));
            receipt.append(quickqash::printing::EscPos::newline());
            for (int r = 0; r < d->model->rowCount(); ++r) {
                QString name = d->model->data(d->model->index(r, 1)).toString();
                QString qtys = d->model->data(d->model->index(r, 2)).toString();
                QString price = d->model->data(d->model->index(r, 3)).toString();
                receipt.append(quickqash::printing::EscPos::text(QString("%1 x%2 %3\n").arg(name, qtys, price)));
            }
            receipt.append(quickqash::printing::EscPos::newline());
            receipt.append(quickqash::printing::EscPos::text(QString("Total: %1\n").arg(QString::number((total_cents/100.0), 'f', 2))));
            receipt.append(quickqash::printing::EscPos::newline());
            receipt.append(quickqash::printing::EscPos::drawerPulse());

            // enqueue receipt for printing (retry queue)
            static quickqash::printing::PrintQueue queue;
            queue.enqueue(receipt);
            d->model->removeRows(0, d->model->rowCount());
            if (d->lbl_subtotal) d->lbl_subtotal->setText("Subtotal: 0.00");
            if (d->lbl_tax) d->lbl_tax->setText("Tax: 0.00");
            if (d->lbl_discount) d->lbl_discount->setText("Discount: 0.00");
            if (d->lbl_total) d->lbl_total->setText("Total: 0.00");
        });
    }

    this->setProperty("_pd", QVariant::fromValue((void*)d));
}

PosView::~PosView() {
    void* vp = this->property("_pd").value<void*>();
    if (vp) delete static_cast<PosViewPrivate*>(vp);
}
