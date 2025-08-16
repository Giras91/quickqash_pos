#include "util/Settings.h"
#include <QSettings>

#include <QCoreApplication>

using namespace quickqash::util;

Settings& Settings::instance() {
    static Settings s;
    return s;
}

Settings::Settings() {}
Settings::~Settings() {}

double Settings::taxRate() const { return m_taxRate; }
void Settings::setTaxRate(double r) { m_taxRate = r; }

QString Settings::printerHost() const { return m_printerHost; }
int Settings::printerPort() const { return m_printerPort; }
void Settings::setPrinterHost(const QString& h) { m_printerHost = h; }
void Settings::setPrinterPort(int p) { m_printerPort = p; }

void Settings::load() {
    QCoreApplication::setApplicationName("QuickQash");
    QCoreApplication::setOrganizationName("QuickQashLtd");
    QSettings s;
    m_taxRate = s.value("taxRate", 0.06).toDouble();
    m_printerHost = s.value("printerHost", QString("127.0.0.1")).toString();
    m_printerPort = s.value("printerPort", 9100).toInt();
}

void Settings::save() {
    QSettings s;
    s.setValue("taxRate", m_taxRate);
    s.setValue("printerHost", m_printerHost);
    s.setValue("printerPort", m_printerPort);
}
