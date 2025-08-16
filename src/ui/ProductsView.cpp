#include "ui/ProductsView.h"
#include <QtUiTools/QUiLoader>
#include <QFile>
#include <QTableView>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include "data/ProductRepo.h"
#include "ui/ProductDialog.h"
#include <QItemSelectionModel>
#include <QDebug>
#include <QMessageBox>
#include <QKeySequence>

using namespace quickqash::ui;
using quickqash::data::ProductRepo;
using quickqash::domain::Product;

class ProductsViewPrivate {
public:
    QWidget* root = nullptr;
    QTableView* table = nullptr;
    QStandardItemModel* model = nullptr;
    QPushButton* btn_add = nullptr;
    QPushButton* btn_edit = nullptr;
    QPushButton* btn_delete = nullptr;
};

ProductsView::ProductsView(QWidget* parent) : QWidget(parent) {
    QUiLoader loader;
    QFile f("ui/ProductsView.ui");
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open ui/ProductsView.ui";
        return;
    }
    QWidget* root = loader.load(&f, this);
    f.close();
    if (!root) {
        qWarning() << "Failed to load ProductsView UI";
        return;
    }

    auto d = new ProductsViewPrivate();
    d->root = root;
    d->table = root->findChild<QTableView*>("table_products");

    d->model = new QStandardItemModel(0, 4, this);
    d->model->setHeaderData(0, Qt::Horizontal, "ID");
    d->model->setHeaderData(1, Qt::Horizontal, "Barcode");
    d->model->setHeaderData(2, Qt::Horizontal, "Name");
    d->model->setHeaderData(3, Qt::Horizontal, "Price");

    if (d->table) {
        d->table->setModel(d->model);
        d->table->horizontalHeader()->setStretchLastSection(true);
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(root);

    auto loadProducts = [&]() {
        d->model->removeRows(0, d->model->rowCount());
        QSqlQuery q(quickqash::data::Db::instance().database());
        if (q.exec("SELECT id, barcode, name, price_cents FROM products ORDER BY id DESC")) {
            while (q.next()) {
                int row = d->model->rowCount();
                d->model->insertRow(row);
                d->model->setData(d->model->index(row, 0), q.value(0).toInt());
                d->model->setData(d->model->index(row, 1), q.value(1).toString());
                d->model->setData(d->model->index(row, 2), q.value(2).toString());
                d->model->setData(d->model->index(row, 3), QString::number(q.value(3).toLongLong() / 100.0, 'f', 2));
            }
        }
    };

    d->btn_add = root->findChild<QPushButton*>("btn_add");
    d->btn_edit = root->findChild<QPushButton*>("btn_edit");
    d->btn_delete = root->findChild<QPushButton*>("btn_delete");

    if (!d->btn_add) {
        d->btn_add = new QPushButton("Add", root);
        QToolBar* tb = root->findChild<QToolBar*>("toolbar");
        if (tb) tb->addWidget(d->btn_add);
    }
    // keyboard shortcuts
    if (d->btn_add) d->btn_add->setShortcut(QKeySequence(Qt::Key_Insert));
    if (d->btn_edit) d->btn_edit->setShortcut(QKeySequence(Qt::Key_F2));
    if (d->btn_delete) d->btn_delete->setShortcut(QKeySequence(Qt::Key_Delete));

    if (d->btn_add) {
        connect(d->btn_add, &QPushButton::clicked, [this, d, loadProducts]() {
            ProductDialog dlg(this);
            if (dlg.exec() == QDialog::Accepted) {
                Product p = dlg.product();
                ProductRepo repo;
                if (repo.save(p)) {
                    loadProducts();
                }
            }
        });
    }

    if (d->btn_edit) {
        connect(d->btn_edit, &QPushButton::clicked, [this, d, loadProducts]() {
            QItemSelectionModel* sel = d->table->selectionModel();
            if (!sel || !sel->hasSelection()) return;
            QModelIndex idx = sel->currentIndex();
            int id = d->model->data(d->model->index(idx.row(), 0)).toInt();
            ProductRepo repo;
            // fetch product by barcode could be improved; simple query by id here
            QSqlQuery q(quickqash::data::Db::instance().database());
            q.prepare("SELECT id, barcode, name, price_cents, stock FROM products WHERE id = :id");
            q.bindValue(":id", id);
            if (!q.exec() || !q.next()) return;
            Product p;
            p.id = q.value(0).toInt();
            p.barcode = q.value(1).toString();
            p.name = q.value(2).toString();
            p.price_cents = q.value(3).toLongLong();
            p.stock = q.value(4).toInt();

            ProductDialog dlg(this);
            dlg.setProduct(p);
            if (dlg.exec() == QDialog::Accepted) {
                Product updated = dlg.product();
                updated.id = p.id;
                if (repo.save(updated)) loadProducts();
            }
        });
    }

    if (d->btn_delete) {
        connect(d->btn_delete, &QPushButton::clicked, [this, d, loadProducts]() {
            QItemSelectionModel* sel = d->table->selectionModel();
            if (!sel || !sel->hasSelection()) return;
            QModelIndex idx = sel->currentIndex();
            int id = d->model->data(d->model->index(idx.row(), 0)).toInt();
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete product",
                "Are you sure you want to delete the selected product?", QMessageBox::Yes|QMessageBox::No);
            if (reply != QMessageBox::Yes) return;
            ProductRepo repo;
            if (repo.deleteById(id)) loadProducts();
        });
    }

    loadProducts();

    this->setProperty("_pd", QVariant::fromValue((void*)d));
}

ProductsView::~ProductsView() {
    void* vp = this->property("_pd").value<void*>();
    if (vp) delete static_cast<ProductsViewPrivate*>(vp);
}
