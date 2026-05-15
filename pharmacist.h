#ifndef PHARMACIST_H
#define PHARMACIST_H
#include "user.h"
class Pharmacist : public User {
public:
    Pharmacist();
    Pharmacist(int id, const QString& username, const QString& password, const QString& fullName);
};
#endif
