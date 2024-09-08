/********************************************************************************
** Form generated from reading UI file 'coralytics.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CORALYTICS_H
#define UI_CORALYTICS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnTanks;
    QPushButton *btnParams;
    QPushButton *btnTrends;
    QPushButton *btnTasks;
    QPushButton *btnSettings;
    QLabel *lblWelcome;
    QLabel *lblSelect;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnTanks = new QPushButton(centralwidget);
        btnTanks->setObjectName("btnTanks");
        btnTanks->setGeometry(QRect(10, 10, 121, 41));
        btnParams = new QPushButton(centralwidget);
        btnParams->setObjectName("btnParams");
        btnParams->setGeometry(QRect(150, 10, 121, 41));
        btnTrends = new QPushButton(centralwidget);
        btnTrends->setObjectName("btnTrends");
        btnTrends->setGeometry(QRect(290, 10, 121, 41));
        btnTasks = new QPushButton(centralwidget);
        btnTasks->setObjectName("btnTasks");
        btnTasks->setGeometry(QRect(430, 10, 121, 41));
        btnSettings = new QPushButton(centralwidget);
        btnSettings->setObjectName("btnSettings");
        btnSettings->setGeometry(QRect(570, 10, 121, 41));
        lblWelcome = new QLabel(centralwidget);
        lblWelcome->setObjectName("lblWelcome");
        lblWelcome->setGeometry(QRect(30, 70, 131, 16));
        lblSelect = new QLabel(centralwidget);
        lblSelect->setObjectName("lblSelect");
        lblSelect->setGeometry(QRect(30, 90, 181, 16));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName("menuEdit");
        menuView = new QMenu(menubar);
        menuView->setObjectName("menuView");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuHelp->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Coralytics", nullptr));
        btnTanks->setText(QCoreApplication::translate("MainWindow", "Tanks", nullptr));
        btnParams->setText(QCoreApplication::translate("MainWindow", "Log Parameters", nullptr));
        btnTrends->setText(QCoreApplication::translate("MainWindow", "View Trends", nullptr));
        btnTasks->setText(QCoreApplication::translate("MainWindow", "Tasks", nullptr));
        btnSettings->setText(QCoreApplication::translate("MainWindow", "Settings", nullptr));
        lblWelcome->setText(QCoreApplication::translate("MainWindow", "Welcome to Coralytics!", nullptr));
        lblSelect->setText(QCoreApplication::translate("MainWindow", "Select a tank to get started", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "Edit", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CORALYTICS_H
