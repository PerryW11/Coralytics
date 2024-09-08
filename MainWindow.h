#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addNewTank();
    void selectTank(QListWidgetItem* item);
    void showTanks();
    void showLogParameters();
    void showViewTrends();
    void showTasks();
    void showSettings();
    void showContextMenu(const QPoint &pos);
    void deleteTank();
    void goBackToTankSelection();

private:
    Ui::MainWindow *ui;
    QListWidget *tankListWidget;
    QPushButton *addTankButton;
    QVBoxLayout *mainLayout;
    QSqlDatabase db;
    QListWidgetItem *rightClickedItem;

    void initializeDatabase();
    void resetDatabase();
    void refreshTankList();
    void loadTankProfiles();
};
#endif // MAINWINDOW_H
