#include <QApplication>
#include <QFile>
#include <QtUiTools/QUiLoader>
#include <QMainWindow>
#include <QWidget>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "data/Db.h"

static bool applyMigrations(const QString& sqlPath) {
    QFile f(sqlPath);
    if (!f.exists()) return true; // nothing to do
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open migration file:" << sqlPath;
        return false;
    }
    QTextStream in(&f);
    QString sql = in.readAll();
    f.close();

    QSqlDatabase db = quickqash::data::Db::instance().database();
    QSqlQuery q(db);
    // Execute the whole SQL script; SQLite supports multiple statements
    if (!q.exec(sql)) {
        qWarning() << "Failed to execute migrations:" << q.lastError().text();
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Open database (pos.db in CWD)
    if (!quickqash::data::Db::instance().open("pos.db")) {
        qCritical() << "Cannot open database.";
        return 1;
    }

    // load persisted settings
    quickqash::util::Settings::instance().load();

    // Apply schema migrations if required
    QString schemaPath = QDir(QCoreApplication::applicationDirPath()).filePath("data/schema.sql");
    if (!applyMigrations(schemaPath)) {
        qCritical() << "Failed to apply migrations.";
        return 1;
    }

    QUiLoader loader;
    QString uiPath = QDir(QCoreApplication::applicationDirPath()).filePath("ui/MainWindow.ui");
    QFile file(uiPath);
    if (!file.open(QFile::ReadOnly)) {
        // fallback to source-relative path when running from IDE
        file.setFileName("ui/MainWindow.ui");
        if (!file.open(QFile::ReadOnly)) {
            qWarning("Failed to open ui/MainWindow.ui");
            return 1;
        }
    }

    QWidget *root = loader.load(&file);
    file.close();
    if (!root) {
        qWarning("Failed to load UI");
        return 1;
    }

    QMainWindow mw;
    mw.setCentralWidget(root);
    // Connect Products button if present
    QPushButton* btn = root->findChild<QPushButton*>("btn_products");
    if (btn) {
        QObject::connect(btn, &QPushButton::clicked, [&mw]() {
            // lazy-create ProductsView
            quickqash::ui::ProductsView* v = new quickqash::ui::ProductsView(&mw);
            v->setAttribute(Qt::WA_DeleteOnClose);
            v->show();
        });
    }
    QPushButton* btnPos = root->findChild<QPushButton*>("btn_pos");
    if (btnPos) {
        QObject::connect(btnPos, &QPushButton::clicked, [&mw]() {
            quickqash::ui::PosView* v = new quickqash::ui::PosView(&mw);
            v->setAttribute(Qt::WA_DeleteOnClose);
            v->show();
        });
    }
    QPushButton* btnSettings = root->findChild<QPushButton*>("btn_settings");
    if (btnSettings) {
        QObject::connect(btnSettings, &QPushButton::clicked, [&mw]() {
            quickqash::ui::SettingsDialog dlg(&mw);
            dlg.exec();
        });
    }
    QPushButton* btnPQ = root->findChild<QPushButton*>("btn_printqueue");
    if (btnPQ) {
        QObject::connect(btnPQ, &QPushButton::clicked, [&mw]() {
            quickqash::ui::PrintQueueView* v = new quickqash::ui::PrintQueueView(&mw);
            v->setAttribute(Qt::WA_DeleteOnClose);
            v->show();
        });
    }
    mw.setWindowTitle("QuickQash POS");
    mw.showMaximized();
    return app.exec();
}
