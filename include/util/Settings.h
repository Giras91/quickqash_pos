#pragma once

#include <QString>

namespace quickqash::util {

class Settings {
public:
    static Settings& instance();

    double taxRate() const; // e.g., 0.06 for 6%
    void setTaxRate(double r);

    QString printerHost() const;
    int printerPort() const;
    void setPrinterHost(const QString& h);
    void setPrinterPort(int p);

    void load();
    void save();

private:
    Settings();
    ~Settings();
    double m_taxRate;
    QString m_printerHost;
    int m_printerPort;
};

}
