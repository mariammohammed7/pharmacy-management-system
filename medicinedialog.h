#ifndef MEDICINEDIALOG_H
#define MEDICINEDIALOG_H

#include <QDialog>
#include "medicine.h"

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QDateEdit;

class MedicineDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MedicineDialog(QWidget* parent = nullptr, const Medicine* existing = nullptr,
                            int suggestedId = 1);
    Medicine medicine() const;

private:
    QSpinBox*       m_id;
    QLineEdit*      m_name;
    QLineEdit*      m_generic;
    QLineEdit*      m_category;
    QLineEdit*      m_unit;
    QSpinBox*       m_stock;
    QSpinBox*       m_minStock;
    QLineEdit*      m_supplier;
    QDoubleSpinBox* m_cost;
    QDateEdit*      m_expiry;
};

#endif
