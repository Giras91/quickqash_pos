#pragma once

#include <QByteArray>
#include <QQueue>
#include <QTimer>
#include <QObject>

namespace quickqash::printing {

class PrintQueue : public QObject {
    Q_OBJECT
public:
    explicit PrintQueue(QObject* parent = nullptr);
    ~PrintQueue();

    void enqueue(const QByteArray& data);
    QStringList persistedFiles() const;
    bool removePersistedFile(const QString& filename);
    void clearAllPersisted();
    void removeByHash(const QByteArray& hashHex);

private slots:
    void processQueue();

private:
    QQueue<QByteArray> m_queue;
    QTimer m_timer;
};

}
