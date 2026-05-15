#include "transaction.h"
#include <QStringList>

Transaction::Transaction() : m_transactionId(0), m_medicineId(0), m_quantity(0), m_unitCost(0), m_pharmacistId(0) {}

Transaction::Transaction(int transactionId, int medicineId, const QString& medicineName,
                         int quantity, double unitCost, const QDateTime& date,
                         int pharmacistId, const QString& pharmacistName)
    : m_transactionId(transactionId), m_medicineId(medicineId),
      m_medicineName(medicineName), m_quantity(quantity),
      m_unitCost(unitCost), m_date(date),
      m_pharmacistId(pharmacistId), m_pharmacistName(pharmacistName) {}

int Transaction::getTransactionId() const { return m_transactionId; }
int Transaction::getMedicineId() const { return m_medicineId; }
QString Transaction::getMedicineName() const { return m_medicineName; }
int Transaction::getQuantity() const { return m_quantity; }
QDateTime Transaction::getDate() const { return m_date; }
double Transaction::getUnitCost() const { return m_unitCost; }
double Transaction::getTotalCost() const { return m_unitCost * m_quantity; }
int Transaction::getPharmacistId() const { return m_pharmacistId; }
QString Transaction::getPharmacistName() const { return m_pharmacistName; }
void Transaction::setTransactionId(int id) { m_transactionId = id; }

QString Transaction::toFileLine() const {
    return QString("%1|%2|%3|%4|%5|%6|%7|%8")
        .arg(m_transactionId).arg(m_medicineId).arg(m_medicineName).arg(m_quantity)
        .arg(m_unitCost, 0, 'f', 2).arg(m_date.toString("yyyy-MM-dd hh:mm"))
        .arg(m_pharmacistId).arg(m_pharmacistName);
}

Transaction Transaction::fromFileLine(const QString& line) {
    QStringList p = line.split('|');
    Transaction t;
    if (p.size() < 8) return t;
    t.m_transactionId = p[0].toInt(); t.m_medicineId = p[1].toInt();
    t.m_medicineName = p[2]; t.m_quantity = p[3].toInt();
    t.m_unitCost = p[4].toDouble();
    t.m_date = QDateTime::fromString(p[5], "yyyy-MM-dd hh:mm");
    t.m_pharmacistId = p[6].toInt(); t.m_pharmacistName = p[7];
    return t;
}
