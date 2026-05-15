#ifndef PHARMACY_H
#define PHARMACY_H

#include <QVector>
#include <QString>
#include "medicine.h"
#include "transaction.h"
#include "user.h"

class Pharmacy
{
public:
    Pharmacy();

    void setMedicinesFile(const QString& path);
    void setUsersFile(const QString& path);
    void setTransactionsFile(const QString& path);
    void setLogsFile(const QString& path);

    bool loadAll();
    bool saveAll() const;
    bool loadMedicines();    bool saveMedicines() const;
    bool loadUsers();        bool saveUsers() const;
    bool loadTransactions(); bool saveTransactions() const;
    bool loadLogs();         bool saveLogs() const;

    const QVector<Medicine>& getMedicines() const;
    QVector<Medicine>& medicines();
    bool addMedicine(const Medicine& m);
    bool removeMedicine(int id);
    Medicine* findMedicineById(int id);
    const Medicine* findMedicineById(int id) const;
    int nextMedicineId() const;

    const QVector<User>& getUsers() const;
    bool addUser(const User& u);
    bool removeUser(int id);
    User* findUserById(int id);
    User* findUserByUsername(const QString& username);
    bool resetUserPassword(int id, const QString& newPass);
    int nextUserId() const;

    const QVector<Transaction>& getTransactions() const;
    void addTransaction(const Transaction& t);
    int nextTransactionId() const;

    const QVector<QString>& getLogs() const;
    void addLog(const QString& message);

private:
    QVector<Medicine> m_medicines;
    QVector<User> m_users;
    QVector<Transaction> m_transactions;
    QVector<QString> m_logs;

    QString m_medicinesFile;
    QString m_usersFile;
    QString m_transactionsFile;
    QString m_logsFile;
};

#endif
