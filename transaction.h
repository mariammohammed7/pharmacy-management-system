#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDateTime>

class Transaction
{
public:
    Transaction();
    Transaction(int transactionId, int medicineId, const QString& medicineName,
                int quantity, double unitCost, const QDateTime& date,
                int pharmacistId, const QString& pharmacistName);

    int getTransactionId() const;
    int getMedicineId() const;
    QString getMedicineName() const;
    int getQuantity() const;
    QDateTime getDate() const;
    double getUnitCost() const;
    double getTotalCost() const;
    int getPharmacistId() const;
    QString getPharmacistName() const;

    void setTransactionId(int id);

    QString toFileLine() const;
    static Transaction fromFileLine(const QString& line);

private:
    int m_transactionId;
    int m_medicineId;
    QString m_medicineName;
    int m_quantity;
    double m_unitCost;
    QDateTime m_date;
    int m_pharmacistId;
    QString m_pharmacistName;
};

#endif
