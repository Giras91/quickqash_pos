#include "printing/EscPos.h"

using namespace quickqash::printing;

QByteArray EscPos::text(const QString& utf8) {
    // Basic implementation: convert text to UTF-8 and append newline
    return utf8.toUtf8();
}

QByteArray EscPos::newline() {
    return QByteArray("\n");
}

QByteArray EscPos::cut() {
    // ESC i (partial) or ESC m (full) in many printers; using GS V 1 for full cut
    QByteArray cmd;
    cmd.append(0x1D); // GS
    cmd.append('V');
    cmd.append(1);
    return cmd;
}

QByteArray EscPos::drawerPulse() {
    QByteArray cmd;
    cmd.append(0x1B); // ESC
    cmd.append('p');
    cmd.append((char)0x00); // m
    cmd.append((char)0x50); // t1 (on time)
    cmd.append((char)0x50); // t2 (off time)
    return cmd;
}

QByteArray EscPos::alignLeft() {
    QByteArray b;
    b.append(0x1B); b.append('a'); b.append((char)0x00);
    return b;
}

QByteArray EscPos::alignCenter() {
    QByteArray b;
    b.append(0x1B); b.append('a'); b.append((char)0x01);
    return b;
}

QByteArray EscPos::alignRight() {
    QByteArray b;
    b.append(0x1B); b.append('a'); b.append((char)0x02);
    return b;
}

QByteArray EscPos::boldOn() {
    QByteArray b;
    b.append(0x1B); b.append('E'); b.append((char)0x01);
    return b;
}

QByteArray EscPos::boldOff() {
    QByteArray b;
    b.append(0x1B); b.append('E'); b.append((char)0x00);
    return b;
}

QByteArray EscPos::columnLine(const QString& left, const QString& right, int width) {
    // basic fixed-width columns using spaces; width is total chars
    QString l = left;
    QString r = right;
    int pad = width - l.size() - r.size();
    if (pad < 1) pad = 1;
    QString line = l + QString(pad, ' ') + r + "\n";
    return line.toUtf8();
}
