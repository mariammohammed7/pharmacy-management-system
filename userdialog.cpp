#include "userdialog.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

UserDialog::UserDialog(QWidget* parent, int suggestedId) : QDialog(parent)
{
    setWindowTitle("Add User");
    setMinimumWidth(360);

    QVBoxLayout* root = new QVBoxLayout(this);
    QFormLayout* form = new QFormLayout;

    m_id = new QSpinBox; m_id->setRange(1, 1000000); m_id->setValue(suggestedId);
    m_username = new QLineEdit;
    m_password = new QLineEdit;
    m_fullName = new QLineEdit;
    m_type = new QComboBox; m_type->addItem("Admin"); m_type->addItem("Pharmacist");
    m_type->setCurrentIndex(1);

    form->addRow("ID:", m_id);
    form->addRow("Username:", m_username);
    form->addRow("Password:", m_password);
    form->addRow("Full name:", m_fullName);
    form->addRow("Type:", m_type);
    root->addLayout(form);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    root->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

User UserDialog::user() const {
    return User(m_id->value(), m_username->text().trimmed(),
                m_password->text(), m_fullName->text().trimmed(),
                User::typeFromString(m_type->currentText()));
}
