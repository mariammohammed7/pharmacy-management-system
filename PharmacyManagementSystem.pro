QT       += core gui widgets
CONFIG   += c++17
TARGET    = PharmacyManagementSystem
TEMPLATE  = app

SOURCES += \
    main.cpp \
    medicine.cpp \
    transaction.cpp \
    user.cpp \
    admin.cpp \
    pharmacist.cpp \
    pharmacy.cpp \
    logindialog.cpp \
    mainwindow.cpp \
    medicinedialog.cpp \
    dispensedialog.cpp \
    userdialog.cpp \
    reportdialog.cpp

HEADERS += \
    medicine.h \
    transaction.h \
    user.h \
    admin.h \
    pharmacist.h \
    pharmacy.h \
    logindialog.h \
    mainwindow.h \
    medicinedialog.h \
    dispensedialog.h \
    userdialog.h \
    reportdialog.h
# WebAssembly asyncify support
wasm: QMAKE_LFLAGS += -sASYNCIFY
