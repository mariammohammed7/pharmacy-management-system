#include "dispensedialog.h"
#include "medicine.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

DispenseDialog::DispenseDialog(const Medicine& med, QWidget* parent)
    : QDialog(parent), m_unitCost(med.getUnitCost())
{
    setWindowTitle("Dispense Medicine");
    setMinimumWidth(360);

    QVBoxLayout* root = new QVBoxLayout(this);
    QFormLayout* form = new QFormLayout;

    QLabel* info = new QLabel(QString("<b>%1</b><br>Available stock: %2<br>Unit cost: %3")
                              .arg(med.getName()).arg(med.getStock())
                              .arg(QString::number(med.getUnitCost(), 'f', 2)));
    root->addWidget(info);

    m_qty = new QSpinBox;
    m_qty->setRange(1, med.getStock() > 0 ? med.getStock() : 1);
    form->addRow("Quantity:", m_qty);

    m_total = new QLabel(QString::number(m_unitCost, 'f', 2));
    form->addRow("Total cost:", m_total);
    root->addLayout(form);

    connect(m_qty, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v){
        m_total->setText(QString::number(v * m_unitCost, 'f', 2));
    });

    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    box->button(QDialogButtonBox::Ok)->setText("Dispense");
    root->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

int DispenseDialog::quantity() const { return m_qty->value(); }
