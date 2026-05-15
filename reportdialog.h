#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>

class QSpinBox;
class QTextEdit;
class Pharmacy;

class ReportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ReportDialog(const Pharmacy* pharmacy, QWidget* parent = nullptr);

private slots:
    void generate();
    void saveToFile();

private:
    const Pharmacy* m_pharmacy;
    QSpinBox*  m_month;
    QSpinBox*  m_year;
    QTextEdit* m_output;
};

#endif
