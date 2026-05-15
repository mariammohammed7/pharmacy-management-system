#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;
class Pharmacy;
class User;

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(Pharmacy* pharmacy, QWidget* parent = nullptr);
    User* authenticatedUser() const;

private slots:
    void tryLogin();

private:
    Pharmacy* m_pharmacy;
    User*     m_user = nullptr;
    QLineEdit* m_username;
    QLineEdit* m_password;
    QComboBox* m_type;
};

#endif
