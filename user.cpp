#include "user.h"
#include <QStringList>

User::User() : m_id(0), m_type(UserType::Pharmacist) {}
User::User(int id, const QString& u, const QString& p, const QString& n, UserType t)
    : m_id(id), m_username(u), m_password(p), m_fullName(n), m_type(t) {}
User::~User() = default;

int User::getId() const { return m_id; }
QString User::getUsername() const { return m_username; }
QString User::getPassword() const { return m_password; }
QString User::getFullName() const { return m_fullName; }
UserType User::getType() const { return m_type; }

void User::setId(int id) { m_id = id; }
void User::setUsername(const QString& u) { m_username = u; }
void User::setPassword(const QString& p) { m_password = p; }
void User::setFullName(const QString& n) { m_fullName = n; }
void User::setType(UserType t) { m_type = t; }

bool User::login(const QString& u, const QString& p) const { return m_username == u && m_password == p; }

bool User::changePassword(const QString& oldPass, const QString& newPass) {
    if (m_password != oldPass || newPass.isEmpty()) return false;
    m_password = newPass; return true;
}

QString User::toFileLine() const {
    return QString("%1|%2|%3|%4|%5").arg(m_id).arg(m_username)
        .arg(m_password).arg(m_fullName).arg(typeToString(m_type));
}
User User::fromFileLine(const QString& line) {
    QStringList p = line.split('|');
    User u;
    if (p.size() < 5) return u;
    u.m_id = p[0].toInt(); u.m_username = p[1]; u.m_password = p[2];
    u.m_fullName = p[3]; u.m_type = typeFromString(p[4]);
    return u;
}
QString User::typeToString(UserType t) { return t == UserType::Admin ? "Admin" : "Pharmacist"; }
UserType User::typeFromString(const QString& s) {
    return s.compare("Admin", Qt::CaseInsensitive) == 0 ? UserType::Admin : UserType::Pharmacist;
}
