#pragma once

#include <QObject>
#include <QList>
#include <QString>

namespace quickqash::services {

class PosService : public QObject {
    Q_OBJECT
public:
    explicit PosService(QObject* parent = nullptr);

    // Calculate totals, apply tax/discount – stubs for now
    qint64 calculateSubtotalCents() const;

private:
    // ...existing code...
};

}
