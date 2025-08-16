#include "printing/PrintQueue.h"
#include "printing/ReceiptPrinter.h"
#include "util/Settings.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCryptographicHash>

using namespace quickqash::printing;

PrintQueue::PrintQueue(QObject* parent) : QObject(parent) {
    m_timer.setInterval(5000); // retry every 5s
    connect(&m_timer, &QTimer::timeout, this, &PrintQueue::processQueue);
    m_timer.start();
    // load persisted queue files
    QDir dir("data/print_queue");
    if (dir.exists()) {
        auto files = dir.entryList(QDir::Files);
        for (const QString& f : files) {
            QFile ff(dir.filePath(f));
            if (ff.open(QIODevice::ReadOnly)) {
                QByteArray b = ff.readAll();
                ff.close();
                m_queue.enqueue(b);
            }
        }
    } else {
        dir.mkpath(".");
    }
}

PrintQueue::~PrintQueue() {
    m_timer.stop();
}

void PrintQueue::enqueue(const QByteArray& data) {
    // persist to disk
    QDir dir("data/print_queue");
    if (!dir.exists()) dir.mkpath(".");
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();
    QString fname = dir.filePath(QString::fromUtf8(hash) + ".bin");
    QFile f(fname);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(data);
        f.close();
        m_queue.enqueue(data);
    } else {
        qWarning() << "PrintQueue: failed to persist queued receipt";
        m_queue.enqueue(data);
    }
}

void PrintQueue::processQueue() {
    if (m_queue.isEmpty()) return;
    QByteArray item = m_queue.head();
    QString err;
    ReceiptPrinter printer;
    if (printer.printReceiptTcp(util::Settings::instance().printerHost(), util::Settings::instance().printerPort(), item, err)) {
        qDebug() << "Printed queued receipt";
    // remove persisted file if exists
    QDir dir("data/print_queue");
    QByteArray hash = QCryptographicHash::hash(item, QCryptographicHash::Sha1).toHex();
    QString fname = dir.filePath(QString::fromUtf8(hash) + ".bin");
    QFile::remove(fname);
    m_queue.dequeue();
    } else {
        qWarning() << "Print queue: failed to print queued receipt:" << err;
        // keep it in queue for next retry
    }
}

QStringList PrintQueue::persistedFiles() const {
    QDir dir("data/print_queue");
    if (!dir.exists()) return {};
    return dir.entryList(QDir::Files);
}

bool PrintQueue::removePersistedFile(const QString& filename) {
    QDir dir("data/print_queue");
    QString fp = dir.filePath(filename);
    return QFile::remove(fp);
}

void PrintQueue::clearAllPersisted() {
    QDir dir("data/print_queue");
    if (!dir.exists()) return;
    auto files = dir.entryList(QDir::Files);
    for (const QString& f : files) QFile::remove(dir.filePath(f));
    m_queue.clear();
}

void PrintQueue::removeByHash(const QByteArray& hashHex) {
    QDir dir("data/print_queue");
    QString fname = dir.filePath(QString::fromUtf8(hashHex) + ".bin");
    QFile::remove(fname);
}
