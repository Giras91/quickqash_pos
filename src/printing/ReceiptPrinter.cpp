#include "printing/ReceiptPrinter.h"
#include <QTcpSocket>

using namespace quickqash::printing;

ReceiptPrinter::ReceiptPrinter() {}
ReceiptPrinter::~ReceiptPrinter() {}

bool ReceiptPrinter::printReceiptTcp(const QString& host, int port, const QByteArray& bytes, QString& outError) {
    QTcpSocket sock;
    sock.connectToHost(host, static_cast<quint16>(port));
    if (!sock.waitForConnected(3000)) {
        outError = sock.errorString();
        return false;
    }
    qint64 written = sock.write(bytes);
    if (written != bytes.size()) {
        outError = "Failed to write all bytes to socket";
        sock.disconnectFromHost();
        return false;
    }
    sock.flush();
    sock.waitForBytesWritten(2000);
    sock.disconnectFromHost();
    return true;
}
