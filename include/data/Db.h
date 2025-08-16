#pragma once

#include <QSqlDatabase>
#include <QString>

namespace quickqash::data {

class Db {
public:
    static Db& instance();
    bool open(const QString& path = "pos.db");
    QSqlDatabase& database();
    void close();

private:
    Db();
    ~Db();
    QSqlDatabase m_db;
};

}
