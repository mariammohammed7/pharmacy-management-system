#include "logindialog.h"
#include "pharmacy.h"
#include "user.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>

LoginDialog::LoginDialog(Pharmacy* pharmacy, QWidget* parent)
    : QDialog(parent), m_pharmacy(pharmacy)
{
    setWindowTitle("Pharmacy — Login");
    setModal(true);
    setMinimumWidth(340);

    QVBoxLayout* root = new QVBoxLayout(this);

    QLabel* title = new QLabel("<h2>Pharmacy Management System</h2>");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    QFormLayout* form = new QFormLayout;
    m_username = new QLineEdit;
    m_username->setText("admin");
    m_password = new QLineEdit;
    m_password->setEchoMode(QLineEdit::Password);
    m_password->setText("admin123");
    m_type = new QComboBox;
    m_type->addItem("Admin");
    m_type->addItem("Pharmacist");
    form->addRow("Username:", m_username);
    form->addRow("Password:", m_password);
    form->addRow("User type:", m_type);
    root->addLayout(form);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* loginBtn = new QPushButton("Login");
    QPushButton* cancelBtn = new QPushButton("Cancel");
    loginBtn->setDefault(true);
    btns->addStretch();
    btns->addWidget(cancelBtn);
    btns->addWidget(loginBtn);
    root->addLayout(btns);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::tryLogin);
    connect(cancelBtn, &QPushButton::clicked, this, &LoginDialog::reject);
}

User* LoginDialog::authenticatedUser() const { return m_user; }

void LoginDialog::tryLogin()
{
    QString u = m_username->text().trimmed();
    QString p = m_password->text();
    UserType wanted = User::typeFromString(m_type->currentText());

    User* found = m_pharmacy->findUserByUsername(u);
    if (!found || !found->login(u, p)) {
        QMessageBox::warning(this, "Login failed", "Invalid username or password.");
        return;
    }
    if (found->getType() != wanted) {
        QMessageBox::warning(this, "Login failed", "Selected user type does not match this account.");
        return;
    }
    m_user = found;
    accept();
}
