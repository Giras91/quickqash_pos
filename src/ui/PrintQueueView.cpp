#include "ui/PrintQueueView.h"
#include <QtUiTools/QUiLoader>
#include <QFile>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include "printing/PrintQueue.h"

using namespace quickqash::ui;
using quickqash::printing::PrintQueue;

class PrintQueueViewPrivate {
public:
    QWidget* root = nullptr;
    QListWidget* list = nullptr;
    QPushButton* btn_remove = nullptr;
    QPushButton* btn_clear = nullptr;
    PrintQueue* queue = nullptr;
};

PrintQueueView::PrintQueueView(QWidget* parent) : QWidget(parent) {
    QUiLoader loader;
    QFile f("ui/PrintQueueView.ui");
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open PrintQueueView UI";
        return;
    }
    QWidget* root = loader.load(&f, this);
    f.close();
    if (!root) {
        qWarning() << "Failed to load PrintQueueView UI";
        return;
    }

    auto d = new PrintQueueViewPrivate();
    d->root = root;
    d->list = root->findChild<QListWidget*>("list_queue");
    d->btn_remove = root->findChild<QPushButton*>("btn_remove");
    d->btn_clear = root->findChild<QPushButton*>("btn_clear");

    d->queue = new PrintQueue(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(root);

    // populate list from persisted files
    QStringList files = d->queue->persistedFiles();
    for (const QString& fName : files) d->list->addItem(fName);

    if (d->btn_remove) connect(d->btn_remove, &QPushButton::clicked, [this, d]() {
        auto item = d->list->currentItem();
        if (!item) return;
        QString fname = item->text();
        if (d->queue->removePersistedFile(fname)) {
            delete d->list->takeItem(d->list->row(item));
        }
    });

    if (d->btn_clear) connect(d->btn_clear, &QPushButton::clicked, [this, d]() {
        d->queue->clearAllPersisted();
        d->list->clear();
    });

    this->setProperty("_pd", QVariant::fromValue((void*)d));
}

PrintQueueView::~PrintQueueView() {
    void* vp = this->property("_pd").value<void*>();
    if (vp) delete static_cast<PrintQueueViewPrivate*>(vp);
}
