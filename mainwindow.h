#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTableWidget;
class QLineEdit;
class QComboBox;
class QTextEdit;
class QTabWidget;
class Pharmacy;
class User;
class Medicine;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(Pharmacy* pharmacy, User* currentUser, QWidget* parent = nullptr);

private slots:
    // Inventory
    void refreshInventory();
    void onAddMedicine();
    void onEditMedicine();
    void onDeleteMedicine();
    void onUpdateStock();
    void onDispense();
    void onSearch();
    void onImportCsv();

    // Transactions
    void refreshTransactions();

    // Alerts
    void refreshLowStock();
    void refreshExpiry();

    // Users
    void refreshUsers();
    void onAddUser();
    void onRemoveUser();
    void onResetPassword();

    // Reports
    void onMonthlyReport();

    // Logs
    void refreshLogs();

    // Misc
    void onChangePassword();
    void onLogout();

private:
    void buildUi();
    Medicine* selectedMedicine();
    int selectedMedicineId() const;
    int selectedUserId() const;

    Pharmacy* m_pharmacy;
    User*     m_user;

    QTabWidget* m_tabs;

    // Inventory tab
    QTableWidget* m_invTable;
    QLineEdit*    m_invSearch;
    QComboBox*    m_invCategory;

    // Transactions tab
    QTableWidget* m_txTable;
    QLineEdit*    m_txSearch;

    // Low stock / expiry
    QTableWidget* m_lowTable;
    QTableWidget* m_expTable;

    // Users
    QTableWidget* m_userTable;

    // Logs
    QTextEdit*    m_logsView;
};

#endif
