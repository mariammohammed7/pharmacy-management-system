#include "pharmacist.h"
Pharmacist::Pharmacist() { m_type = UserType::Pharmacist; }
Pharmacist::Pharmacist(int id, const QString& u, const QString& p, const QString& n)
    : User(id, u, p, n, UserType::Pharmacist) {}
