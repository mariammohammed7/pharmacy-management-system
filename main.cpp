#include <QApplication>
#include "logindialog.h"
#include "mainwindow.h"
#include "pharmacy.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Pharmacy Management System");

    Pharmacy pharmacy;
    pharmacy.loadAll();

    while (true) {
        LoginDialog dlg(&pharmacy);
        if (dlg.exec() != QDialog::Accepted) break;

        User* u = dlg.authenticatedUser();
        if (!u) break;

        pharmacy.addLog(QString("Login: %1 (%2)")
                        .arg(u->getUsername(), User::typeToString(u->getType())));
        pharmacy.saveLogs();

        MainWindow w(&pharmacy, u);
        w.show();
        app.exec(); // returns when main window closes / user logs out
    }
    return 0;
}
