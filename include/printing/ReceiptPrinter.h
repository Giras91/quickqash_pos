#pragma once

#include <QString>

namespace quickqash::printing {

class ReceiptPrinter {
public:
    ReceiptPrinter();
    ~ReceiptPrinter();

    bool printReceiptTcp(const QString& host, int port, const QByteArray& bytes, QString& outError);
};

}
