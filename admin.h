#ifndef ADMIN_H
#define ADMIN_H
#include "user.h"
class Admin : public User {
public:
    Admin();
    Admin(int id, const QString& username, const QString& password, const QString& fullName);
};
#endif
