#include "admin.h"
Admin::Admin() { m_type = UserType::Admin; }
Admin::Admin(int id, const QString& u, const QString& p, const QString& n)
    : User(id, u, p, n, UserType::Admin) {}
