#include <QtTest>
#include <QCoreApplication>
#include "data/Db.h"
#include "data/ProductRepo.h"

class TestCheckout : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        QCoreApplication::setOrganizationName("QuickQashLtd");
        QCoreApplication::setApplicationName("QuickQashTests");
        quickqash::data::Db::instance().open("test_pos.db");
        QFile f("data/schema.sql");
        QVERIFY(f.open(QFile::ReadOnly));
        QByteArray sql = f.readAll();
        QSqlQuery q(quickqash::data::Db::instance().database());
        QVERIFY(q.exec(sql));
    }

    void cleanupTestCase() {
        quickqash::data::Db::instance().close();
        QFile::remove("test_pos.db");
    }

    void testCheckoutDecrementsStock() {
        quickqash::domain::Product p;
        p.barcode = "CHK1";
        p.name = "ChkProd";
        p.price_cents = 500;
        p.stock = 5;
        quickqash::data::ProductRepo repo;
        QVERIFY(repo.save(p));

        // perform a checkout similar to PosView logic
        QSqlDatabase db = quickqash::data::Db::instance().database();
        QVERIFY(db.isOpen());
        QSqlQuery q(db);
        QVERIFY(q.exec("BEGIN IMMEDIATE;"));
        QVERIFY(q.exec("INSERT INTO sales (total_cents, paid_cents, cashier) VALUES (500, 500, 'test')"));
        qint64 saleId = q.lastInsertId().toLongLong();
        QVERIFY(saleId > 0);
        QSqlQuery q1(db);
        q1.prepare("INSERT INTO sale_items (sale_id, product_id, qty, price_cents) VALUES (:sale, (SELECT id FROM products WHERE barcode = :barcode), :qty, :price)");
        q1.bindValue(":sale", saleId);
        q1.bindValue(":barcode", QString("CHK1"));
        q1.bindValue(":qty", 2);
        q1.bindValue(":price", 500);
        QVERIFY(q1.exec());
        QSqlQuery qdec(db);
        qdec.prepare("UPDATE products SET stock = stock - :qty WHERE id = (SELECT id FROM products WHERE barcode = :barcode) AND stock >= :qty");
        qdec.bindValue(":qty", 2);
        qdec.bindValue(":barcode", QString("CHK1"));
        QVERIFY(qdec.exec());
        QCOMPARE(qdec.numRowsAffected(), 1);
        QVERIFY(db.commit());

        // verify stock reduced
        auto found = repo.byBarcode("CHK1");
        QVERIFY(found.has_value());
        QCOMPARE(found->stock, 3);
    }
};

QTEST_MAIN(TestCheckout)
#include "test_checkout.moc"
