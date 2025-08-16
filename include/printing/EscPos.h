#pragma once

#include <QtGlobal>
#include <QByteArray>
#include <QString>

namespace quickqash::printing {

class EscPos {
public:
    static QByteArray text(const QString& utf8);
    static QByteArray newline();
    static QByteArray cut();
    static QByteArray drawerPulse();
    static QByteArray alignLeft();
    static QByteArray alignCenter();
    static QByteArray alignRight();
    static QByteArray boldOn();
    static QByteArray boldOff();
    static QByteArray columnLine(const QString& left, const QString& right, int width = 40);
};

}
