#include "data/ProductRepo.h"
#include "data/Db.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

using namespace quickqash::data;

ProductRepo::ProductRepo() {}

std::optional<quickqash::domain::Product> ProductRepo::byBarcode(const QString& barcode) {
    QSqlDatabase db = Db::instance().database();
    if (!db.isOpen()) return std::nullopt;

    QSqlQuery q(db);
    q.prepare("SELECT id, barcode, name, price_cents, stock FROM products WHERE barcode = :barcode LIMIT 1");
    q.bindValue(":barcode", barcode);
    if (!q.exec()) {
        qWarning() << "ProductRepo byBarcode failed:" << q.lastError().text();
        return std::nullopt;
    }
    if (q.next()) {
        quickqash::domain::Product p;
        p.id = q.value(0).toInt();
        p.barcode = q.value(1).toString();
        p.name = q.value(2).toString();
        p.price_cents = q.value(3).toLongLong();
        p.stock = q.value(4).toInt();
        return p;
    }
    return std::nullopt;
}

bool ProductRepo::save(const quickqash::domain::Product& p) {
    QSqlDatabase db = Db::instance().database();
    if (!db.isOpen()) return false;

    QSqlQuery q(db);
    if (p.id == 0) {
        q.prepare("INSERT INTO products (barcode, name, price_cents, stock) VALUES (:barcode, :name, :price, :stock)");
        q.bindValue(":barcode", p.barcode);
        q.bindValue(":name", p.name);
        q.bindValue(":price", p.price_cents);
        q.bindValue(":stock", p.stock);
        if (!q.exec()) {
            qWarning() << "Insert product failed:" << q.lastError().text();
            return false;
        }
        return true;
    } else {
        q.prepare("UPDATE products SET barcode = :barcode, name = :name, price_cents = :price, stock = :stock WHERE id = :id");
        q.bindValue(":barcode", p.barcode);
        q.bindValue(":name", p.name);
        q.bindValue(":price", p.price_cents);
        q.bindValue(":stock", p.stock);
        q.bindValue(":id", p.id);
        if (!q.exec()) {
            qWarning() << "Update product failed:" << q.lastError().text();
            return false;
        }
        return true;
    }
}

bool ProductRepo::deleteById(int id) {
    QSqlDatabase db = Db::instance().database();
    if (!db.isOpen()) return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM products WHERE id = :id");
    q.bindValue(":id", id);
    if (!q.exec()) {
        qWarning() << "Delete product failed:" << q.lastError().text();
        return false;
    }
    return true;
}
