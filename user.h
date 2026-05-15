#ifndef USER_H
#define USER_H

#include <QString>

enum class UserType { Admin, Pharmacist };

class User
{
public:
    User();
    User(int id, const QString& username, const QString& password,
         const QString& fullName, UserType type);
    virtual ~User();

    int getId() const;
    QString getUsername() const;
    QString getPassword() const;
    QString getFullName() const;
    UserType getType() const;

    void setId(int id);
    void setUsername(const QString& u);
    void setPassword(const QString& p);
    void setFullName(const QString& n);
    void setType(UserType t);

    virtual bool login(const QString& username, const QString& password) const;
    bool changePassword(const QString& oldPass, const QString& newPass);

    QString toFileLine() const;
    static User fromFileLine(const QString& line);
    static QString typeToString(UserType t);
    static UserType typeFromString(const QString& s);

protected:
    int m_id;
    QString m_username;
    QString m_password;
    QString m_fullName;
    UserType m_type;
};

#endif
