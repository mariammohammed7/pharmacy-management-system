#include "reportdialog.h"
#include "pharmacy.h"
#include "transaction.h"
#include "medicine.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QPair>
#include <QVector>
#include <algorithm>

ReportDialog::ReportDialog(const Pharmacy* pharmacy, QWidget* parent)
    : QDialog(parent), m_pharmacy(pharmacy)
{
    setWindowTitle("Monthly Report");
    resize(640, 540);

    QVBoxLayout* root = new QVBoxLayout(this);
    QFormLayout* form = new QFormLayout;
    QDate today = QDate::currentDate();
    m_month = new QSpinBox; m_month->setRange(1, 12); m_month->setValue(today.month());
    m_year  = new QSpinBox; m_year->setRange(2000, 2100); m_year->setValue(today.year());
    form->addRow("Month:", m_month);
    form->addRow("Year:",  m_year);
    root->addLayout(form);

    QHBoxLayout* btns = new QHBoxLayout;
    QPushButton* gen = new QPushButton("Generate");
    QPushButton* save= new QPushButton("Save to file…");
    QPushButton* close= new QPushButton("Close");
    btns->addWidget(gen); btns->addWidget(save); btns->addStretch(); btns->addWidget(close);
    root->addLayout(btns);

    m_output = new QTextEdit;
    m_output->setReadOnly(true);
    m_output->setFontFamily("monospace");
    root->addWidget(m_output, 1);

    connect(gen, &QPushButton::clicked, this, &ReportDialog::generate);
    connect(save, &QPushButton::clicked, this, &ReportDialog::saveToFile);
    connect(close, &QPushButton::clicked, this, &QDialog::accept);

    generate();
}

void ReportDialog::generate()
{
    int month = m_month->value();
    int year  = m_year->value();
    double totalRevenue = 0.0;
    QMap<int,int> unitsByMed;
    QMap<int,QString> nameByMed;
    int txCount = 0;

    for (const Transaction& t : m_pharmacy->getTransactions()) {
        QDate d = t.getDate().date();
        if (d.month() == month && d.year() == year) {
            totalRevenue += t.getTotalCost();
            unitsByMed[t.getMedicineId()] += t.getQuantity();
            nameByMed[t.getMedicineId()]   = t.getMedicineName();
            ++txCount;
        }
    }

    QString s;
    QTextStream out(&s);
    out << "Monthly Report   "
        << QString("%1-%2\n").arg(year, 4, 10, QChar('0')).arg(month, 2, 10, QChar('0'));
    out << "==============================================\n";
    out << "Total transactions: " << txCount << "\n";
    out << "Total revenue:      " << QString::number(totalRevenue, 'f', 2) << "\n\n";

    QVector<QPair<int,int>> pairs;
    for (auto it = unitsByMed.constBegin(); it != unitsByMed.constEnd(); ++it)
        pairs.append({it.key(), it.value()});
    std::sort(pairs.begin(), pairs.end(),
              [](const QPair<int,int>& a, const QPair<int,int>& b){ return a.second > b.second; });

    out << "Units dispensed per medicine:\n";
    for (const auto& p : pairs)
        out << "  " << nameByMed.value(p.first)
            << " (ID " << p.first << "): " << p.second << "\n";

    out << "\nTop 5 most-dispensed medicines:\n";
    for (int i = 0; i < pairs.size() && i < 5; ++i)
        out << "  " << (i+1) << ") " << nameByMed.value(pairs[i].first)
            << " — " << pairs[i].second << " units\n";

    out << "\nLow stock / expiry warnings:\n";
    for (const Medicine& m : m_pharmacy->getMedicines()) {
        if (m.isLowStock())
            out << "  [LOW]     " << m.getName()
                << " (" << m.getStock() << "/" << m.getMinStock() << ")\n";
        if (m.isExpired())
            out << "  [EXPIRED] " << m.getName()
                << " expired " << m.getExpiryDate().toString("yyyy-MM-dd") << "\n";
        else if (m.isExpiringSoon(30))
            out << "  [SOON]    " << m.getName()
                << " expires " << m.getExpiryDate().toString("yyyy-MM-dd") << "\n";
    }
    m_output->setPlainText(s);
}

void ReportDialog::saveToFile()
{
    QString suggested = QString("report_%1_%2.txt")
                          .arg(m_year->value(), 4, 10, QChar('0'))
                          .arg(m_month->value(), 2, 10, QChar('0'));
    QString path = QFileDialog::getSaveFileName(this, "Save report", suggested, "Text (*.txt)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QTextStream out(&f);
        out << m_output->toPlainText();
    }
}
