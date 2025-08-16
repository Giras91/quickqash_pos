#include <QtTest>
#include <QCoreApplication>
#include "data/Db.h"
#include "data/ProductRepo.h"

class TestProductRepo : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        QCoreApplication::setOrganizationName("QuickQashLtd");
        QCoreApplication::setApplicationName("QuickQashTests");
        quickqash::data::Db::instance().open("test_pos.db");
        // apply schema
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

    void testSaveAndByBarcode() {
        quickqash::domain::Product p;
        p.barcode = "TEST123";
        p.name = "Test Product";
        p.price_cents = 199;
        p.stock = 10;
        quickqash::data::ProductRepo repo;
        QVERIFY(repo.save(p));
        auto opt = repo.byBarcode("TEST123");
        QVERIFY(opt.has_value());
        QCOMPARE(opt->name, QString("Test Product"));
    }
};

QTEST_MAIN(TestProductRepo)
#include "test_productrepo.moc"
