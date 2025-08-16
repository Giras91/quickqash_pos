#include "services/PosService.h"

using namespace quickqash::services;

PosService::PosService(QObject* parent) : QObject(parent) {
}

qint64 PosService::calculateSubtotalCents() const {
    return 0; // stub
}
