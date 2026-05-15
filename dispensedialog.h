#ifndef DISPENSEDIALOG_H
#define DISPENSEDIALOG_H

#include <QDialog>

class QSpinBox;
class QLabel;
class Medicine;

class DispenseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DispenseDialog(const Medicine& med, QWidget* parent = nullptr);
    int quantity() const;

private:
    QSpinBox* m_qty;
    QLabel*   m_total;
    double    m_unitCost;
};

#endif
