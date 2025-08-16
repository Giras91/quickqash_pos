#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "ui/ProductsView.h"
#include <QtUiTools/QUiLoader>
#include <QFile>
#include <QWidget>

// Minimal placeholder for any MainWindow-specific helpers.
// UI is loaded at runtime from ui/MainWindow.ui via QUiLoader in main.cpp

// ...existing code...

static void showProducts(QWidget* parent) {
	quickqash::ui::ProductsView* v = new quickqash::ui::ProductsView(parent);
	v->setAttribute(Qt::WA_DeleteOnClose);
	v->show();
}
