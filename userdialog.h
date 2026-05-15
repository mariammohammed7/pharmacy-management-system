#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include "user.h"

class QLineEdit;
class QSpinBox;
class QComboBox;

class UserDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UserDialog(QWidget* parent = nullptr, int suggestedId = 1);
    User user() const;

private:
    QSpinBox*  m_id;
    QLineEdit* m_username;
    QLineEdit* m_password;
    QLineEdit* m_fullName;
    QComboBox* m_type;
};

#endif
