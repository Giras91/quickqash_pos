#include "data/Db.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QDebug>

using namespace quickqash::data;

Db::Db() {
}

Db::~Db() {
    if (m_db.isOpen()) m_db.close();
}

Db& Db::instance() {
    static Db inst;
    return inst;
}

bool Db::open(const QString& path) {
    QString dbPath = path;
    // Ensure directory exists
    QFileInfo fi(dbPath);
    QDir dir = fi.dir();
    if (!dir.exists()) dir.mkpath(".");

    if (QSqlDatabase::contains("quickqash_connection")) {
        m_db = QSqlDatabase::database("quickqash_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "quickqash_connection");
        m_db.setDatabaseName(dbPath);
    }

    if (!m_db.open()) {
        qCritical() << "Failed to open DB:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery q(m_db);
    // Pragmas for SQLite tuning
    q.exec("PRAGMA foreign_keys = ON;");
    q.exec("PRAGMA journal_mode = WAL;");
    q.exec("PRAGMA synchronous = NORMAL;");

    return true;
}

QSqlDatabase& Db::database() {
    return m_db;
}

void Db::close() {
    if (m_db.isOpen()) m_db.close();
}
