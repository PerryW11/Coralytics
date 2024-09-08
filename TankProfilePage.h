#ifndef TANKPROFILEPAGE_H
#define TANKPROFILEPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSqlDatabase>

class TankProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit TankProfilePage(const QString &tankName, QWidget *parent = nullptr);

signals:
    void backToTankSelectionRequested();

private slots:
    void logNewParameters();
    void loadParameters();
    void viewAllParameters();
    void addTask();
    void showCompletedTasks();
    void editTask(const QString &description);
    void completeTask(const QString &description);
    void deleteTask(const QString &description);
    void viewCreatureProfile(const QString &name);
    void viewAllActiveTasks();
    void addLivestock();
    void goBackToTankSelection();
    void viewParameterTrends();
    void printLivestockTableSchema();

private:
    QString tankName;
    QSqlDatabase db;

    QLabel *tankNameLabel;
    QLabel *tankTypeLabel;
    QLabel *tankVolumeLabel;
    QLabel *parametersLabel;
    QPushButton *viewAllParametersButton;
    QPushButton *viewParameterTrendsButton;
    QPushButton *logNewParametersButton;
    QListWidget *taskListWidget;
    QListWidget *livestockListWidget;
    QPushButton *addTaskButton;
    QPushButton *viewAllTasksButton;
    QPushButton *showCompletedTasksButton;
    QPushButton *addLivestockButton;
    QPushButton *backToTankSelectionButton;

    bool viewingCompletedTasks = false;  // Tracks whether we are viewing completed tasks
    QPushButton *showUpcomingTasksButton;  // Button to toggle between showing completed and upcoming tasks

    void loadTankData();
    void loadTasks();
    void loadLivestock();
    void logCurrentParameters(int tankId);
    void loadCompletedTasks();  // Method to load completed tasks
};

#endif // TANKPROFILEPAGE_H
