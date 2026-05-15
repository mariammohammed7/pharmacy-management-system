#include "medicinedialog.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>

MedicineDialog::MedicineDialog(QWidget* parent, const Medicine* existing, int suggestedId)
    : QDialog(parent)
{
    setWindowTitle(existing ? "Edit Medicine" : "Add Medicine");
    setMinimumWidth(420);

    QVBoxLayout* root = new QVBoxLayout(this);
    QFormLayout* form = new QFormLayout;

    m_id        = new QSpinBox; m_id->setRange(1, 1000000);
    m_name      = new QLineEdit;
    m_generic   = new QLineEdit;
    m_category  = new QLineEdit;
    m_unit      = new QLineEdit;
    m_stock     = new QSpinBox; m_stock->setRange(0, 1000000);
    m_minStock  = new QSpinBox; m_minStock->setRange(0, 1000000);
    m_supplier  = new QLineEdit;
    m_cost      = new QDoubleSpinBox; m_cost->setRange(0, 1000000); m_cost->setDecimals(2);
    m_expiry    = new QDateEdit; m_expiry->setCalendarPopup(true);
    m_expiry->setDisplayFormat("yyyy-MM-dd");
    m_expiry->setDate(QDate::currentDate().addYears(1));

    if (existing) {
        m_id->setValue(existing->getId());
        m_id->setEnabled(false);
        m_name->setText(existing->getName());
        m_generic->setText(existing->getGenericName());
        m_category->setText(existing->getCategory());
        m_unit->setText(existing->getUnit());
        m_stock->setValue(existing->getStock());
        m_minStock->setValue(existing->getMinStock());
        m_supplier->setText(existing->getSupplier());
        m_cost->setValue(existing->getUnitCost());
        if (existing->getExpiryDate().isValid())
            m_expiry->setDate(existing->getExpiryDate());
    } else {
        m_id->setValue(suggestedId);
    }

    form->addRow("ID:", m_id);
    form->addRow("Name:", m_name);
    form->addRow("Generic name:", m_generic);
    form->addRow("Category:", m_category);
    form->addRow("Unit:", m_unit);
    form->addRow("Current stock:", m_stock);
    form->addRow("Minimum stock:", m_minStock);
    form->addRow("Supplier:", m_supplier);
    form->addRow("Unit cost:", m_cost);
    form->addRow("Expiry date:", m_expiry);
    root->addLayout(form);

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    root->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

Medicine MedicineDialog::medicine() const
{
    return Medicine(m_id->value(), m_name->text().trimmed(),
                    m_generic->text().trimmed(), m_category->text().trimmed(),
                    m_unit->text().trimmed(), m_stock->value(), m_minStock->value(),
                    m_supplier->text().trimmed(), m_cost->value(), m_expiry->date());
}
