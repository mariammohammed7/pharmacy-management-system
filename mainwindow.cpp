#include "mainwindow.h"
#include "pharmacy.h"
#include "user.h"
#include "medicine.h"
#include "transaction.h"
#include "medicinedialog.h"
#include "dispensedialog.h"
#include "userdialog.h"
#include "reportdialog.h"

#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QColor>
#include <QBrush>
#include <QTextEdit>
#include <QApplication>
#include <QSet>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

static QTableWidgetItem* roItem(const QString& s) {
    QTableWidgetItem* it = new QTableWidgetItem(s);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    return it;
}

MainWindow::MainWindow(Pharmacy* pharmacy, User* currentUser, QWidget* parent)
    : QMainWindow(parent), m_pharmacy(pharmacy), m_user(currentUser)
{
    setWindowTitle(QString("Pharmacy Management System — %1 (%2)")
                   .arg(m_user->getFullName(), User::typeToString(m_user->getType())));
    resize(1100, 680);
    buildUi();

    refreshInventory();
    refreshTransactions();
    refreshLowStock();
    refreshExpiry();
    if (m_user->getType() == UserType::Admin) {
        refreshUsers();
        refreshLogs();
    }
}

void MainWindow::buildUi()
{
    bool isAdmin = (m_user->getType() == UserType::Admin);

    // Menu
    QMenu* file = menuBar()->addMenu("&File");
    QAction* changePw = file->addAction("Change my password…");
    QAction* logout   = file->addAction("Log out");
    file->addSeparator();
    QAction* quit     = file->addAction("Quit");
    connect(changePw, &QAction::triggered, this, &MainWindow::onChangePassword);
    connect(logout,   &QAction::triggered, this, &MainWindow::onLogout);
    connect(quit,     &QAction::triggered, qApp, &QApplication::quit);

    m_tabs = new QTabWidget;
    setCentralWidget(m_tabs);

    // ---- Inventory tab ----
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(tab);

        QHBoxLayout* topRow = new QHBoxLayout;
        m_invSearch = new QLineEdit; m_invSearch->setPlaceholderText("Search by name, generic, supplier…");
        m_invCategory = new QComboBox; m_invCategory->addItem("All categories");
        QPushButton* searchBtn = new QPushButton("Search");
        QPushButton* clearBtn  = new QPushButton("Clear");
        topRow->addWidget(new QLabel("Search:"));
        topRow->addWidget(m_invSearch, 1);
        topRow->addWidget(m_invCategory);
        topRow->addWidget(searchBtn);
        topRow->addWidget(clearBtn);
        v->addLayout(topRow);

        m_invTable = new QTableWidget(0, 10);
        QStringList hdr{"ID","Name","Generic","Category","Unit","Stock","Min","Supplier","Cost","Expiry"};
        m_invTable->setHorizontalHeaderLabels(hdr);
        m_invTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_invTable->setSelectionMode(QAbstractItemView::SingleSelection);
        m_invTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_invTable->horizontalHeader()->setStretchLastSection(true);
        v->addWidget(m_invTable, 1);

        QHBoxLayout* btnRow = new QHBoxLayout;
        QPushButton* addBtn    = new QPushButton("Add medicine");
        QPushButton* editBtn   = new QPushButton("Edit");
        QPushButton* delBtn    = new QPushButton("Delete");
        QPushButton* stockBtn  = new QPushButton("Update stock");
        QPushButton* dispBtn   = new QPushButton("Dispense");
        QPushButton* importBtn = new QPushButton("Import CSV…");
        addBtn->setEnabled(isAdmin);
        editBtn->setEnabled(isAdmin);
        delBtn->setEnabled(isAdmin);
        stockBtn->setEnabled(isAdmin);
        importBtn->setEnabled(isAdmin);
        btnRow->addWidget(addBtn); btnRow->addWidget(editBtn); btnRow->addWidget(delBtn);
        btnRow->addWidget(stockBtn); btnRow->addWidget(importBtn);
        btnRow->addStretch(); btnRow->addWidget(dispBtn);
        v->addLayout(btnRow);

        connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearch);
        connect(m_invSearch, &QLineEdit::returnPressed, this, &MainWindow::onSearch);
        connect(m_invCategory, &QComboBox::currentTextChanged, this, [this](const QString&){ onSearch(); });
        connect(clearBtn, &QPushButton::clicked, this, [this](){
            m_invSearch->clear(); m_invCategory->setCurrentIndex(0); refreshInventory();
        });
        connect(addBtn,   &QPushButton::clicked, this, &MainWindow::onAddMedicine);
        connect(editBtn,  &QPushButton::clicked, this, &MainWindow::onEditMedicine);
        connect(delBtn,   &QPushButton::clicked, this, &MainWindow::onDeleteMedicine);
        connect(stockBtn, &QPushButton::clicked, this, &MainWindow::onUpdateStock);
        connect(dispBtn,  &QPushButton::clicked, this, &MainWindow::onDispense);
        connect(importBtn,&QPushButton::clicked, this, &MainWindow::onImportCsv);

        m_tabs->addTab(tab, "Inventory");
    }

    // ---- Transactions tab ----
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(tab);
        m_txTable = new QTableWidget(0, 7);
        m_txTable->setHorizontalHeaderLabels({"Tx ID","Date","Medicine","Qty","Unit cost","Total","Pharmacist"});
        m_txTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_txTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_txTable->horizontalHeader()->setStretchLastSection(true);
        v->addWidget(m_txTable, 1);
        QPushButton* refreshBtn = new QPushButton("Refresh");
        v->addWidget(refreshBtn, 0, Qt::AlignRight);
        connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshTransactions);
        m_tabs->addTab(tab, "Transactions");
    }

    // ---- Low stock tab ----
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(tab);
        m_lowTable = new QTableWidget(0, 5);
        m_lowTable->setHorizontalHeaderLabels({"ID","Name","Stock","Min","Supplier"});
        m_lowTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_lowTable->horizontalHeader()->setStretchLastSection(true);
        v->addWidget(m_lowTable, 1);
        m_tabs->addTab(tab, "Low Stock");
    }

    // ---- Expiry tab ----
    {
        QWidget* tab = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(tab);
        m_expTable = new QTableWidget(0, 5);
        m_expTable->setHorizontalHeaderLabels({"ID","Name","Expiry","Status","Stock"});
        m_expTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_expTable->horizontalHeader()->setStretchLastSection(true);
        v->addWidget(m_expTable, 1);
        m_tabs->addTab(tab, "Expiry Alerts");
    }

    // ---- Reports tab (admin) ----
    if (isAdmin) {
        QWidget* tab = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(tab);
        v->addWidget(new QLabel("Generate sales / inventory reports for any month."));
        QPushButton* monthly = new QPushButton("Monthly report…");
        v->addWidget(monthly, 0, Qt::AlignLeft);
        v->addStretch();
        connect(monthly, &QPushButton::clicked, this, &MainWindow::onMonthlyReport);
        m_tabs->addTab(tab, "Reports");
    }

    // ---- Users tab (admin) ----
    if (isAdmin) {
        QWidget* tab = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(tab);
        m_userTable = new QTableWidget(0, 4);
        m_userTable->setHorizontalHeaderLabels({"ID","Username","Full name","Type"});
        m_userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_userTable->horizontalHeader()->setStretchLastSection(true);
        v->addWidget(m_userTable, 1);
        QHBoxLayout* row = new QHBoxLayout;
        QPushButton* addBtn = new QPushButton("Add user");
        QPushButton* delBtn = new QPushButton("Remove user");
        QPushButton* pwBtn  = new QPushButton("Reset password");
        row->addWidget(addBtn); row->addWidget(delBtn); row->addWidget(pwBtn); row->addStretch();
        v->addLayout(row);
        connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddUser);
        connect(delBtn, &QPushButton::clicked, this, &MainWindow::onRemoveUser);
        connect(pwBtn,  &QPushButton::clicked, this, &MainWindow::onResetPassword);
        m_tabs->addTab(tab, "Users");
    }

    // ---- Logs tab (admin) ----
    if (isAdmin) {
        QWidget* tab = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(tab);
        m_logsView = new QTextEdit; m_logsView->setReadOnly(true);
        m_logsView->setFontFamily("monospace");
        v->addWidget(m_logsView, 1);
        QPushButton* refreshBtn = new QPushButton("Refresh");
        v->addWidget(refreshBtn, 0, Qt::AlignRight);
        connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshLogs);
        m_tabs->addTab(tab, "Activity Log");
    }

    statusBar()->showMessage(QString("Logged in as %1").arg(m_user->getUsername()));
}

// ---------- helpers ----------
int MainWindow::selectedMedicineId() const {
    int r = m_invTable->currentRow();
    if (r < 0) return -1;
    return m_invTable->item(r, 0)->text().toInt();
}
Medicine* MainWindow::selectedMedicine() {
    int id = selectedMedicineId();
    if (id < 0) return nullptr;
    return m_pharmacy->findMedicineById(id);
}
int MainWindow::selectedUserId() const {
    int r = m_userTable->currentRow();
    if (r < 0) return -1;
    return m_userTable->item(r, 0)->text().toInt();
}

// ---------- Inventory ----------
void MainWindow::refreshInventory()
{
    QString prevCat = m_invCategory->currentText();
    QSet<QString> cats;
    for (const Medicine& m : m_pharmacy->getMedicines()) cats.insert(m.getCategory());
    m_invCategory->blockSignals(true);
    m_invCategory->clear();
    m_invCategory->addItem("All categories");
    QStringList sorted;
    for (const QString& c : cats) sorted.append(c);
    sorted.sort();
    for (const QString& c : sorted) m_invCategory->addItem(c);
    int idx = m_invCategory->findText(prevCat);
    m_invCategory->setCurrentIndex(idx >= 0 ? idx : 0);
    m_invCategory->blockSignals(false);

    onSearch();
}

void MainWindow::onSearch()
{
    QString q = m_invSearch->text().trimmed().toLower();
    QString cat = m_invCategory->currentText();

    const QVector<Medicine>& meds = m_pharmacy->getMedicines();
    m_invTable->setRowCount(0);
    for (const Medicine& m : meds) {
        if (cat != "All categories" && m.getCategory() != cat) continue;
        if (!q.isEmpty()) {
            QString hay = (m.getName()+" "+m.getGenericName()+" "+m.getSupplier()).toLower();
            if (!hay.contains(q)) continue;
        }
        int r = m_invTable->rowCount();
        m_invTable->insertRow(r);
        m_invTable->setItem(r, 0, roItem(QString::number(m.getId())));
        m_invTable->setItem(r, 1, roItem(m.getName()));
        m_invTable->setItem(r, 2, roItem(m.getGenericName()));
        m_invTable->setItem(r, 3, roItem(m.getCategory()));
        m_invTable->setItem(r, 4, roItem(m.getUnit()));
        m_invTable->setItem(r, 5, roItem(QString::number(m.getStock())));
        m_invTable->setItem(r, 6, roItem(QString::number(m.getMinStock())));
        m_invTable->setItem(r, 7, roItem(m.getSupplier()));
        m_invTable->setItem(r, 8, roItem(QString::number(m.getUnitCost(), 'f', 2)));
        m_invTable->setItem(r, 9, roItem(m.getExpiryDate().toString("yyyy-MM-dd")));

        if (m.isLowStock())
            for (int c = 0; c < m_invTable->columnCount(); ++c)
                m_invTable->item(r, c)->setBackground(QColor(255, 235, 200));
        if (m.isExpired())
            for (int c = 0; c < m_invTable->columnCount(); ++c)
                m_invTable->item(r, c)->setBackground(QColor(255, 200, 200));
    }
    m_invTable->resizeColumnsToContents();
}

void MainWindow::onAddMedicine()
{
    MedicineDialog dlg(this, nullptr, m_pharmacy->nextMedicineId());
    if (dlg.exec() != QDialog::Accepted) return;
    Medicine m = dlg.medicine();
    if (m.getName().isEmpty()) {
        QMessageBox::warning(this, "Add medicine", "Name cannot be empty."); return;
    }
    if (!m_pharmacy->addMedicine(m)) {
        QMessageBox::warning(this, "Add medicine", "A medicine with that ID already exists."); return;
    }
    m_pharmacy->addLog(QString("Added medicine: %1 (ID %2)").arg(m.getName()).arg(m.getId()));
    m_pharmacy->saveMedicines(); m_pharmacy->saveLogs();
    refreshInventory(); refreshLowStock(); refreshExpiry();
    if (m_user->getType() == UserType::Admin) refreshLogs();
}

void MainWindow::onEditMedicine()
{
    Medicine* m = selectedMedicine();
    if (!m) { QMessageBox::information(this, "Edit", "Select a medicine first."); return; }
    MedicineDialog dlg(this, m);
    if (dlg.exec() != QDialog::Accepted) return;
    *m = dlg.medicine();
    m_pharmacy->addLog(QString("Edited medicine: %1 (ID %2)").arg(m->getName()).arg(m->getId()));
    m_pharmacy->saveMedicines(); m_pharmacy->saveLogs();
    refreshInventory(); refreshLowStock(); refreshExpiry();
    if (m_user->getType() == UserType::Admin) refreshLogs();
}

void MainWindow::onDeleteMedicine()
{
    int id = selectedMedicineId();
    if (id < 0) { QMessageBox::information(this, "Delete", "Select a medicine first."); return; }
    Medicine* m = m_pharmacy->findMedicineById(id);
    if (!m) return;
    if (QMessageBox::question(this, "Delete medicine",
            QString("Delete '%1' (ID %2)?").arg(m->getName()).arg(id))
        != QMessageBox::Yes) return;
    QString name = m->getName();
    m_pharmacy->removeMedicine(id);
    m_pharmacy->addLog(QString("Deleted medicine: %1 (ID %2)").arg(name).arg(id));
    m_pharmacy->saveMedicines(); m_pharmacy->saveLogs();
    refreshInventory(); refreshLowStock(); refreshExpiry();
    if (m_user->getType() == UserType::Admin) refreshLogs();
}

void MainWindow::onUpdateStock()
{
    Medicine* m = selectedMedicine();
    if (!m) { QMessageBox::information(this, "Update stock", "Select a medicine first."); return; }
    bool ok = false;
    int delta = QInputDialog::getInt(this, "Restock",
        QString("How many units to ADD to '%1'? (current: %2)")
            .arg(m->getName()).arg(m->getStock()),
        0, -100000, 100000, 1, &ok);
    if (!ok || delta == 0) return;
    if (!m->updateStock(delta)) {
        QMessageBox::warning(this, "Update stock", "Not enough stock to remove that quantity.");
        return;
    }
    m_pharmacy->addLog(QString("Stock %1 by %2 for %3 (now %4)")
        .arg(delta>0?"increased":"decreased").arg(qAbs(delta))
        .arg(m->getName()).arg(m->getStock()));
    m_pharmacy->saveMedicines(); m_pharmacy->saveLogs();
    refreshInventory(); refreshLowStock();
    if (m_user->getType() == UserType::Admin) refreshLogs();
}

void MainWindow::onDispense()
{
    Medicine* m = selectedMedicine();
    if (!m) { QMessageBox::information(this, "Dispense", "Select a medicine first."); return; }
    if (m->getStock() <= 0) {
        QMessageBox::warning(this, "Dispense", "This medicine is out of stock."); return;
    }
    if (m->isExpired()) {
        QMessageBox::warning(this, "Dispense", "This medicine is expired and cannot be dispensed."); return;
    }
    DispenseDialog dlg(*m, this);
    if (dlg.exec() != QDialog::Accepted) return;
    int qty = dlg.quantity();
    if (qty > m->getStock()) {
        QMessageBox::warning(this, "Dispense", "Not enough stock."); return;
    }
    m->updateStock(-qty);
    Transaction t(m_pharmacy->nextTransactionId(), m->getId(), m->getName(),
                  qty, m->getUnitCost(), QDateTime::currentDateTime(),
                  m_user->getId(), m_user->getUsername());
    m_pharmacy->addTransaction(t);
    m_pharmacy->addLog(QString("Dispensed %1 x %2 (total %3) by %4")
        .arg(qty).arg(m->getName())
        .arg(QString::number(qty * m->getUnitCost(), 'f', 2))
        .arg(m_user->getUsername()));
    m_pharmacy->saveMedicines();
    m_pharmacy->saveTransactions();
    m_pharmacy->saveLogs();
    QMessageBox::information(this, "Dispense",
        QString("Dispensed %1 unit(s). Total: %2")
            .arg(qty).arg(QString::number(qty * m->getUnitCost(), 'f', 2)));
    refreshInventory(); refreshTransactions(); refreshLowStock();
    if (m_user->getType() == UserType::Admin) refreshLogs();
}

// ---------- CSV import ----------
void MainWindow::onImportCsv()
{
    QString path = QFileDialog::getOpenFileName(this, "Import medicines from CSV",
                                                QString(), "CSV files (*.csv);;All files (*)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Import CSV", "Could not open the file."); return;
    }
    QTextStream in(&f);
    int added = 0, updated = 0, skipped = 0, lineNum = 0;
    bool first = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        ++lineNum;
        // strip UTF-8 BOM if present on first line
        if (line.startsWith(QChar(0xFEFF))) line.remove(0, 1);
        if (line.trimmed().isEmpty()) continue;
        // Skip header row if it doesn't start with a digit
        if (first) {
            first = false;
            QString f0 = line.section(',', 0, 0).trimmed();
            bool isNumeric = false; f0.toInt(&isNumeric);
            if (!isNumeric) continue;
        }
        // CSV split (no quoted-comma support — Phase 1 CSV is plain)
        QStringList p = line.split(',');
        if (p.size() < 11) { ++skipped; continue; }
        bool ok1=false,ok2=false,ok3=false,ok4=false;
        int    id        = p[0].trimmed().toInt(&ok1);
        QString name     = p[1].trimmed();
        QString generic  = p[2].trimmed();
        QString category = p[3].trimmed();
        QString unit     = p[4].trimmed();
        int    stock     = p[5].trimmed().toInt(&ok2);
        int    minStock  = p[6].trimmed().toInt(&ok3);
        // p[7] = monthlyUsage (ignored — not in Medicine class)
        QString supplier = p[8].trimmed();
        double cost      = p[9].trimmed().toDouble(&ok4);
        QDate  expiry    = QDate::fromString(p[10].trimmed(), "yyyy-MM-dd");
        if (!ok1 || !ok2 || !ok3 || !ok4 || name.isEmpty() || !expiry.isValid()) {
            ++skipped; continue;
        }
        Medicine m(id, name, generic, category, unit, stock, minStock, supplier, cost, expiry);
        Medicine* existing = m_pharmacy->findMedicineById(id);
        if (existing) { *existing = m; ++updated; }
        else          { m_pharmacy->addMedicine(m); ++added; }
    }
    m_pharmacy->addLog(QString("Imported CSV '%1' — added %2, updated %3, skipped %4")
        .arg(path).arg(added).arg(updated).arg(skipped));
    m_pharmacy->saveMedicines(); m_pharmacy->saveLogs();
    refreshInventory(); refreshLowStock(); refreshExpiry();
    if (m_user->getType() == UserType::Admin) refreshLogs();
    QMessageBox::information(this, "Import CSV",
        QString("Done.\n\nAdded: %1\nUpdated: %2\nSkipped: %3")
            .arg(added).arg(updated).arg(skipped));
}

// ---------- Transactions ----------
void MainWindow::refreshTransactions()
{
    const QVector<Transaction>& txs = m_pharmacy->getTransactions();
    m_txTable->setRowCount(0);
    for (const Transaction& t : txs) {
        int r = m_txTable->rowCount(); m_txTable->insertRow(r);
        m_txTable->setItem(r, 0, roItem(QString::number(t.getTransactionId())));
        m_txTable->setItem(r, 1, roItem(t.getDate().toString("yyyy-MM-dd hh:mm")));
        m_txTable->setItem(r, 2, roItem(t.getMedicineName()));
        m_txTable->setItem(r, 3, roItem(QString::number(t.getQuantity())));
        m_txTable->setItem(r, 4, roItem(QString::number(t.getUnitCost(), 'f', 2)));
        m_txTable->setItem(r, 5, roItem(QString::number(t.getTotalCost(), 'f', 2)));
        m_txTable->setItem(r, 6, roItem(t.getPharmacistName()));
    }
    m_txTable->resizeColumnsToContents();
}

// ---------- Low stock ----------
void MainWindow::refreshLowStock()
{
    m_lowTable->setRowCount(0);
    for (const Medicine& m : m_pharmacy->getMedicines()) {
        if (!m.isLowStock()) continue;
        int r = m_lowTable->rowCount(); m_lowTable->insertRow(r);
        m_lowTable->setItem(r, 0, roItem(QString::number(m.getId())));
        m_lowTable->setItem(r, 1, roItem(m.getName()));
        m_lowTable->setItem(r, 2, roItem(QString::number(m.getStock())));
        m_lowTable->setItem(r, 3, roItem(QString::number(m.getMinStock())));
        m_lowTable->setItem(r, 4, roItem(m.getSupplier()));
    }
    m_lowTable->resizeColumnsToContents();
}

// ---------- Expiry ----------
void MainWindow::refreshExpiry()
{
    m_expTable->setRowCount(0);
    for (const Medicine& m : m_pharmacy->getMedicines()) {
        QString status;
        if (m.isExpired()) status = "EXPIRED";
        else if (m.isExpiringSoon(30)) status = "Expires soon";
        else continue;
        int r = m_expTable->rowCount(); m_expTable->insertRow(r);
        m_expTable->setItem(r, 0, roItem(QString::number(m.getId())));
        m_expTable->setItem(r, 1, roItem(m.getName()));
        m_expTable->setItem(r, 2, roItem(m.getExpiryDate().toString("yyyy-MM-dd")));
        m_expTable->setItem(r, 3, roItem(status));
        m_expTable->setItem(r, 4, roItem(QString::number(m.getStock())));
    }
    m_expTable->resizeColumnsToContents();
}

// ---------- Users ----------
void MainWindow::refreshUsers()
{
    if (!m_userTable) return;
    m_userTable->setRowCount(0);
    for (const User& u : m_pharmacy->getUsers()) {
        int r = m_userTable->rowCount(); m_userTable->insertRow(r);
        m_userTable->setItem(r, 0, roItem(QString::number(u.getId())));
        m_userTable->setItem(r, 1, roItem(u.getUsername()));
        m_userTable->setItem(r, 2, roItem(u.getFullName()));
        m_userTable->setItem(r, 3, roItem(User::typeToString(u.getType())));
    }
    m_userTable->resizeColumnsToContents();
}

void MainWindow::onAddUser()
{
    UserDialog dlg(this, m_pharmacy->nextUserId());
    if (dlg.exec() != QDialog::Accepted) return;
    User u = dlg.user();
    if (u.getUsername().isEmpty() || u.getPassword().isEmpty()) {
        QMessageBox::warning(this, "Add user", "Username and password are required."); return;
    }
    if (!m_pharmacy->addUser(u)) {
        QMessageBox::warning(this, "Add user", "ID or username already in use."); return;
    }
    m_pharmacy->addLog(QString("Added user: %1 (%2)")
        .arg(u.getUsername(), User::typeToString(u.getType())));
    m_pharmacy->saveUsers(); m_pharmacy->saveLogs();
    refreshUsers(); refreshLogs();
}

void MainWindow::onRemoveUser()
{
    int id = selectedUserId();
    if (id < 0) { QMessageBox::information(this, "Remove user", "Select a user."); return; }
    if (id == m_user->getId()) {
        QMessageBox::warning(this, "Remove user", "You cannot remove yourself."); return;
    }
    User* u = m_pharmacy->findUserById(id);
    if (!u) return;
    if (QMessageBox::question(this, "Remove user",
        QString("Remove user '%1'?").arg(u->getUsername())) != QMessageBox::Yes) return;
    QString name = u->getUsername();
    m_pharmacy->removeUser(id);
    m_pharmacy->addLog(QString("Removed user: %1").arg(name));
    m_pharmacy->saveUsers(); m_pharmacy->saveLogs();
    refreshUsers(); refreshLogs();
}

void MainWindow::onResetPassword()
{
    int id = selectedUserId();
    if (id < 0) { QMessageBox::information(this, "Reset password", "Select a user."); return; }
    bool ok = false;
    QString np = QInputDialog::getText(this, "Reset password",
        "New password:", QLineEdit::Password, "", &ok);
    if (!ok || np.isEmpty()) return;
    if (!m_pharmacy->resetUserPassword(id, np)) {
        QMessageBox::warning(this, "Reset password", "Failed to reset password."); return;
    }
    m_pharmacy->addLog(QString("Reset password for user ID %1").arg(id));
    m_pharmacy->saveUsers(); m_pharmacy->saveLogs();
    refreshLogs();
    QMessageBox::information(this, "Reset password", "Password updated.");
}

// ---------- Reports ----------
void MainWindow::onMonthlyReport()
{
    ReportDialog dlg(m_pharmacy, this);
    dlg.exec();
}

// ---------- Logs ----------
void MainWindow::refreshLogs()
{
    if (!m_logsView) return;
    QString s;
    const auto& logs = m_pharmacy->getLogs();
    for (const QString& l : logs) s += l + "\n";
    m_logsView->setPlainText(s);
}

// ---------- Account ----------
void MainWindow::onChangePassword()
{
    bool ok = false;
    QString cur = QInputDialog::getText(this, "Change password",
        "Current password:", QLineEdit::Password, "", &ok);
    if (!ok) return;
    if (cur != m_user->getPassword()) {
        QMessageBox::warning(this, "Change password", "Current password is incorrect."); return;
    }
    QString np = QInputDialog::getText(this, "Change password",
        "New password:", QLineEdit::Password, "", &ok);
    if (!ok || np.isEmpty()) return;
    m_user->setPassword(np);
    m_pharmacy->addLog(QString("User %1 changed their password").arg(m_user->getUsername()));
    m_pharmacy->saveUsers(); m_pharmacy->saveLogs();
    QMessageBox::information(this, "Change password", "Password updated.");
}

void MainWindow::onLogout()
{
    m_pharmacy->addLog(QString("Logout: %1").arg(m_user->getUsername()));
    m_pharmacy->saveLogs();
    close();
    qApp->quit();
}
