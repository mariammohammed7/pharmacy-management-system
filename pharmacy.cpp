#include "pharmacy.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

Pharmacy::Pharmacy()
    : m_medicinesFile("medicines.txt"),
      m_usersFile("users.txt"),
      m_transactionsFile("transactions.txt"),
      m_logsFile("logs.txt") {}

void Pharmacy::setMedicinesFile(const QString& p)    { m_medicinesFile = p; }
void Pharmacy::setUsersFile(const QString& p)        { m_usersFile = p; }
void Pharmacy::setTransactionsFile(const QString& p) { m_transactionsFile = p; }
void Pharmacy::setLogsFile(const QString& p)         { m_logsFile = p; }

bool Pharmacy::loadAll() {
    bool ok = true;
    ok &= loadMedicines(); ok &= loadUsers();
    ok &= loadTransactions(); ok &= loadLogs();
    saveUsers(); // persist any auto-seeded default accounts
    return ok;
}
bool Pharmacy::saveAll() const {
    bool ok = true;
    ok &= saveMedicines(); ok &= saveUsers();
    ok &= saveTransactions(); ok &= saveLogs();
    return ok;
}

bool Pharmacy::loadMedicines() {
    m_medicines.clear();
    QFile f(m_medicinesFile);
    if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString l = in.readLine().trimmed();
        if (!l.isEmpty()) m_medicines.append(Medicine::fromFileLine(l));
    }
    return true;
}
bool Pharmacy::saveMedicines() const {
    QFile f(m_medicinesFile);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;
    QTextStream out(&f);
    for (const Medicine& m : m_medicines) out << m.toFileLine() << "\n";
    return true;
}

bool Pharmacy::loadUsers() {
    m_users.clear();
    QFile f(m_usersFile);
    if (!f.exists()) {
        m_users.append(User(1, "admin",      "admin123",  "Default Admin",      UserType::Admin));
        m_users.append(User(2, "pharmacist", "pharma123", "Default Pharmacist", UserType::Pharmacist));
        return true;
    }
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString l = in.readLine().trimmed();
        if (!l.isEmpty()) m_users.append(User::fromFileLine(l));
    }
    bool hasAdmin = false, hasPharma = false;
    for (const User& u : m_users) {
        if (u.getUsername().compare("admin",      Qt::CaseInsensitive) == 0) hasAdmin  = true;
        if (u.getUsername().compare("pharmacist", Qt::CaseInsensitive) == 0) hasPharma = true;
    }
    if (!hasAdmin)
        m_users.append(User(nextUserId(), "admin",      "admin123",  "Default Admin",      UserType::Admin));
    if (!hasPharma)
        m_users.append(User(nextUserId(), "pharmacist", "pharma123", "Default Pharmacist", UserType::Pharmacist));
    return true;
}
bool Pharmacy::saveUsers() const {
    QFile f(m_usersFile);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;
    QTextStream out(&f);
    for (const User& u : m_users) out << u.toFileLine() << "\n";
    return true;
}

bool Pharmacy::loadTransactions() {
    m_transactions.clear();
    QFile f(m_transactionsFile);
    if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString l = in.readLine().trimmed();
        if (!l.isEmpty()) m_transactions.append(Transaction::fromFileLine(l));
    }
    return true;
}
bool Pharmacy::saveTransactions() const {
    QFile f(m_transactionsFile);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;
    QTextStream out(&f);
    for (const Transaction& t : m_transactions) out << t.toFileLine() << "\n";
    return true;
}

bool Pharmacy::loadLogs() {
    m_logs.clear();
    QFile f(m_logsFile);
    if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString l = in.readLine();
        if (!l.trimmed().isEmpty()) m_logs.append(l);
    }
    return true;
}
bool Pharmacy::saveLogs() const {
    QFile f(m_logsFile);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;
    QTextStream out(&f);
    for (const QString& l : m_logs) out << l << "\n";
    return true;
}

const QVector<Medicine>& Pharmacy::getMedicines() const { return m_medicines; }
QVector<Medicine>& Pharmacy::medicines() { return m_medicines; }

bool Pharmacy::addMedicine(const Medicine& m) {
    if (findMedicineById(m.getId())) return false;
    m_medicines.append(m); return true;
}
bool Pharmacy::removeMedicine(int id) {
    for (int i = 0; i < m_medicines.size(); ++i)
        if (m_medicines[i].getId() == id) { m_medicines.removeAt(i); return true; }
    return false;
}
Medicine* Pharmacy::findMedicineById(int id) {
    for (Medicine& m : m_medicines) if (m.getId() == id) return &m;
    return nullptr;
}
const Medicine* Pharmacy::findMedicineById(int id) const {
    for (const Medicine& m : m_medicines) if (m.getId() == id) return &m;
    return nullptr;
}
int Pharmacy::nextMedicineId() const {
    int mx = 0;
    for (const Medicine& m : m_medicines) if (m.getId() > mx) mx = m.getId();
    return mx + 1;
}

const QVector<User>& Pharmacy::getUsers() const { return m_users; }
bool Pharmacy::addUser(const User& u) {
    if (findUserById(u.getId()) || findUserByUsername(u.getUsername())) return false;
    m_users.append(u); return true;
}
bool Pharmacy::removeUser(int id) {
    for (int i = 0; i < m_users.size(); ++i)
        if (m_users[i].getId() == id) { m_users.removeAt(i); return true; }
    return false;
}
User* Pharmacy::findUserById(int id) {
    for (User& u : m_users) if (u.getId() == id) return &u;
    return nullptr;
}
User* Pharmacy::findUserByUsername(const QString& un) {
    for (User& u : m_users) if (u.getUsername().compare(un, Qt::CaseInsensitive) == 0) return &u;
    return nullptr;
}
bool Pharmacy::resetUserPassword(int id, const QString& np) {
    User* u = findUserById(id);
    if (!u || np.isEmpty()) return false;
    u->setPassword(np); return true;
}
int Pharmacy::nextUserId() const {
    int mx = 0; for (const User& u : m_users) if (u.getId() > mx) mx = u.getId();
    return mx + 1;
}

const QVector<Transaction>& Pharmacy::getTransactions() const { return m_transactions; }
void Pharmacy::addTransaction(const Transaction& t) { m_transactions.append(t); }
int Pharmacy::nextTransactionId() const {
    int mx = 0;
    for (const Transaction& t : m_transactions)
        if (t.getTransactionId() > mx) mx = t.getTransactionId();
    return mx + 1;
}

const QVector<QString>& Pharmacy::getLogs() const { return m_logs; }
void Pharmacy::addLog(const QString& msg) {
    QString s = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_logs.append(QString("[%1] %2").arg(s, msg));
}
