#include "TankProfilePage.h"
#include "LogParametersDialog.h"
#include "ViewAllParametersDialog.h"
#include "TrendsDialog.h"
#include "TaskDialog.h"
#include "TaskItemWidget.h"
#include "CreateCreatureDialog.h"
#include "CreatureProfilePage.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

TankProfilePage::TankProfilePage(const QString &tankName, QWidget *parent)
    : QWidget(parent), tankName(tankName)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    backToTankSelectionButton = new QPushButton("Back to Tank Selection", this);
    connect(backToTankSelectionButton, &QPushButton::clicked, this, &TankProfilePage::goBackToTankSelection);
    mainLayout->addWidget(backToTankSelectionButton);

    QHBoxLayout *infoLayout = new QHBoxLayout();

    QVBoxLayout *tankInfoLayout = new QVBoxLayout();
    tankNameLabel = new QLabel(tankName, this);
    QFont tankNameFont = tankNameLabel->font();
    tankNameFont.setPointSize(16);
    tankNameFont.setBold(true);
    tankNameLabel->setFont(tankNameFont);
    tankInfoLayout->addWidget(tankNameLabel);

    tankTypeLabel = new QLabel(this);
    tankInfoLayout->addWidget(tankTypeLabel);

    tankVolumeLabel = new QLabel(this);
    tankInfoLayout->addWidget(tankVolumeLabel);

    // Add the tank info layout to the info layout
    infoLayout->addLayout(tankInfoLayout);

    // Parameters at a Glance Section
    QVBoxLayout *parametersLayout = new QVBoxLayout();
    QLabel *parametersAtGlanceLabel = new QLabel("Parameters at a glance", this);
    parametersLayout->addWidget(parametersAtGlanceLabel);

    parametersLabel = new QLabel(this);
    parametersLayout->addWidget(parametersLabel);

    // Add the parameters layout to the info layout
    infoLayout->addLayout(parametersLayout);

    // Add the info layout to the main layout
    mainLayout->addLayout(infoLayout);

    // Rest of the UI elements below the tank info and parameters
    viewAllParametersButton = new QPushButton("View All Parameters", this);
    connect(viewAllParametersButton, &QPushButton::clicked, this, &TankProfilePage::viewAllParameters);
    mainLayout->addWidget(viewAllParametersButton);

    viewParameterTrendsButton = new QPushButton("View Parameter Trends", this);
    connect(viewParameterTrendsButton, &QPushButton::clicked, this, &TankProfilePage::viewParameterTrends);
    mainLayout->addWidget(viewParameterTrendsButton);

    logNewParametersButton = new QPushButton("Log New Parameters", this);
    connect(logNewParametersButton, &QPushButton::clicked, this, &TankProfilePage::logNewParameters);
    mainLayout->addWidget(logNewParametersButton);

    // Task List
    QLabel *tasksLabel = new QLabel("Upcoming Tasks for " + tankName, this);
    QFont sectionFont = tasksLabel->font();
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);
    tasksLabel->setFont(sectionFont);
    mainLayout->addWidget(tasksLabel);

    taskListWidget = new QListWidget(this);
    mainLayout->addWidget(taskListWidget);

    addTaskButton = new QPushButton("[+] Add Task", this);
    connect(addTaskButton, &QPushButton::clicked, this, &TankProfilePage::addTask);
    mainLayout->addWidget(addTaskButton);

    viewAllTasksButton = new QPushButton("View All Active Tasks", this);
    connect(viewAllTasksButton, &QPushButton::clicked, this, &TankProfilePage::viewAllActiveTasks);
    mainLayout->addWidget(viewAllTasksButton);

    showCompletedTasksButton = new QPushButton("[Show Completed Tasks]", this);
    connect(showCompletedTasksButton, &QPushButton::clicked, this, &TankProfilePage::showCompletedTasks);
    mainLayout->addWidget(showCompletedTasksButton);

    // Livestock Inventory
    QLabel *livestockLabel = new QLabel("Livestock Inventory", this);
    livestockLabel->setFont(sectionFont);
    mainLayout->addWidget(livestockLabel);

    livestockListWidget = new QListWidget(this);
    mainLayout->addWidget(livestockListWidget);

    addLivestockButton = new QPushButton("Add Livestock", this);
    connect(addLivestockButton, &QPushButton::clicked, this, &TankProfilePage::addLivestock);
    mainLayout->addWidget(addLivestockButton);

    QSqlQuery testQuery;
    if (testQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='livestock'")) {
        if (testQuery.next()) {
            qDebug() << "Table 'livestock' exists.";
        } else {
            qDebug() << "Table 'livestock' does not exist.";
        }
    } else {
        qDebug() << "Error checking for table:" << testQuery.lastError();
    }


    loadTankData();
    loadTasks();
}

void TankProfilePage::addTask()
{
    TaskDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString description = dialog.getDescription();
        QDate dueDate = dialog.getDueDate();
        int recurrenceInterval = dialog.getRecurrenceInterval();
        QString recurrenceType = dialog.getRecurrenceType();

        QSqlQuery query;
        query.prepare("INSERT INTO tasks (tank_id, description, due_date, is_recurring, recurrence_interval, recurrence_type) VALUES ((SELECT id FROM tanks WHERE name = :name), :description, :due_date, :is_recurring, :recurrence_interval, :recurrence_type)");
        query.bindValue(":name", tankName);
        query.bindValue(":description", description);
        query.bindValue(":due_date", dueDate.toString("yyyy-MM-dd"));
        query.bindValue(":is_recurring", recurrenceType != "None" ? 1 : 0);
        query.bindValue(":recurrence_interval", recurrenceType != "None" ? recurrenceInterval : 0);
        query.bindValue(":recurrence_type", recurrenceType != "None" ? recurrenceType : "");

        if (query.exec()) {
            qDebug() << "Task successfully added: " << description << ", due: " << dueDate.toString("yyyy-MM-dd");

            // Reload the tasks to reflect the newly added task
            loadTasks();

            // Ensure the task list is showing "upcoming tasks" and update the button label
            if (viewingCompletedTasks) {
                showCompletedTasks();  // Toggle back to upcoming tasks
            } else {
                showCompletedTasksButton->setText("[Show Completed Tasks]");
            }
        } else {
            qDebug() << "Error adding task: " << query.lastError();
        }
    }
}


void TankProfilePage::editTask(const QString &description)
{
    QSqlQuery query;
    query.prepare("SELECT due_date, recurrence_interval, recurrence_type FROM tasks WHERE description = :description AND tank_id = (SELECT id FROM tanks WHERE name = :name) ORDER BY due_date DESC LIMIT 1");
    query.bindValue(":description", description);
    query.bindValue(":name", tankName);

    if (query.exec() && query.next()) {
        QDate dueDate = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
        int recurrenceInterval = query.value(1).toInt();
        QString recurrenceType = query.value(2).toString();

        TaskDialog dialog(this);
        dialog.setDescription(description);
        dialog.setDueDate(dueDate);
        dialog.setRecurrenceInterval(recurrenceInterval);
        dialog.setRecurrenceType(recurrenceType);

        if (dialog.exec() == QDialog::Accepted) {
            QString newDescription = dialog.getDescription();
            QDate newDueDate = dialog.getDueDate();
            int newRecurrenceInterval = dialog.getRecurrenceInterval();
            QString newRecurrenceType = dialog.getRecurrenceType();

            // Update the task in the database
            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE tasks SET description = :description, due_date = :due_date, recurrence_interval = :recurrence_interval, recurrence_type = :recurrence_type WHERE description = :old_description AND tank_id = (SELECT id FROM tanks WHERE name = :name)");
            updateQuery.bindValue(":description", newDescription);
            updateQuery.bindValue(":due_date", newDueDate.toString("yyyy-MM-dd"));
            updateQuery.bindValue(":recurrence_interval", newRecurrenceInterval);
            updateQuery.bindValue(":recurrence_type", newRecurrenceType);
            updateQuery.bindValue(":old_description", description);
            updateQuery.bindValue(":name", tankName);

            if (updateQuery.exec()) {
                qDebug() << "Task successfully updated: " << newDescription << ", due: " << newDueDate.toString("yyyy-MM-dd");
                loadTasks();
            } else {
                qDebug() << "Error updating task: " << updateQuery.lastError();
            }
        }
    } else {
        qDebug() << "Error retrieving task for editing: " << query.lastError();
    }
}


void TankProfilePage::showCompletedTasks()
{
    if (viewingCompletedTasks) {
        // Switch to showing upcoming tasks
        loadTasks();  // Load upcoming tasks
        showCompletedTasksButton->setText("[Show Completed Tasks]");
        viewingCompletedTasks = false;
    } else {
        // Show completed tasks
        loadCompletedTasks();  // Load completed tasks
        showCompletedTasksButton->setText("[Show Upcoming Tasks]");
        viewingCompletedTasks = true;
    }
}

void TankProfilePage::loadCompletedTasks()
{
    // Clear the current task list and load completed tasks
    taskListWidget->clear();
    QSqlQuery query;
    query.prepare("SELECT description, due_date, completion_date FROM tasks WHERE tank_id = (SELECT id FROM tanks WHERE name = :name) AND completion_date IS NOT NULL ORDER BY completion_date DESC");
    query.bindValue(":name", tankName);

    if (query.exec()) {
        while (query.next()) {
            QString description = query.value(0).toString();
            QDate dueDate = QDate::fromString(query.value(1).toString(), "yyyy-MM-dd");
            QDate completionDate = QDate::fromString(query.value(2).toString(), "yyyy-MM-dd");
            QString taskText = description + " (Due: " + dueDate.toString("yyyy-MM-dd") + ", Completed: " + completionDate.toString("yyyy-MM-dd") + ")";
            QListWidgetItem *item = new QListWidgetItem(taskText);
            taskListWidget->addItem(item);
        }
    } else {
        qDebug() << "Error loading completed tasks: " << query.lastError();
    }
}



void TankProfilePage::completeTask(const QString &description)
{
    // Mark the task as completed by setting the completion date
    QSqlQuery query;
    query.prepare("UPDATE tasks SET completion_date = :completion_date WHERE description = :description AND tank_id = (SELECT id FROM tanks WHERE name = :name) AND completion_date IS NULL");
    query.bindValue(":completion_date", QDate::currentDate().toString("yyyy-MM-dd"));
    query.bindValue(":description", description);
    query.bindValue(":name", tankName);

    if (query.exec()) {
        qDebug() << "Task marked as completed: " << description;

        // Check if the task is recurring
        QSqlQuery recurrenceQuery;
        recurrenceQuery.prepare("SELECT recurrence_interval, recurrence_type, due_date FROM tasks WHERE description = :description AND tank_id = (SELECT id FROM tanks WHERE name = :name) AND is_recurring = 1 ORDER BY due_date DESC LIMIT 1");
        recurrenceQuery.bindValue(":description", description);
        recurrenceQuery.bindValue(":name", tankName);

        if (recurrenceQuery.exec() && recurrenceQuery.next()) {
            int interval = recurrenceQuery.value(0).toInt();
            QString recurrenceType = recurrenceQuery.value(1).toString();
            QDate previousDueDate = QDate::fromString(recurrenceQuery.value(2).toString(), "yyyy-MM-dd");

            if (interval > 0 && recurrenceType != "None") {
                QDate newDueDate = previousDueDate;

                if (recurrenceType == "Daily") {
                    newDueDate = previousDueDate.addDays(interval);
                } else if (recurrenceType == "Weekly") {
                    newDueDate = previousDueDate.addDays(interval * 7);
                } else if (recurrenceType == "Monthly") {
                    newDueDate = previousDueDate.addMonths(interval);
                }

                // Insert the new recurring task with the updated due date
                QSqlQuery newTaskQuery;
                newTaskQuery.prepare("INSERT INTO tasks (tank_id, description, due_date, is_recurring, recurrence_interval, recurrence_type) VALUES ((SELECT id FROM tanks WHERE name = :name), :description, :due_date, 1, :interval, :type)");
                newTaskQuery.bindValue(":name", tankName);
                newTaskQuery.bindValue(":description", description);
                newTaskQuery.bindValue(":due_date", newDueDate.toString("yyyy-MM-dd"));
                newTaskQuery.bindValue(":interval", interval);
                newTaskQuery.bindValue(":type", recurrenceType);

                if (newTaskQuery.exec()) {
                    qDebug() << "New recurring task created: " << description << ", due: " << newDueDate.toString("yyyy-MM-dd");
                } else {
                    qDebug() << "Error creating new recurring task: " << newTaskQuery.lastError();
                }
            }
        }

        // Log the entire tasks table after completion
        QSqlQuery logQuery;
        if (logQuery.exec("SELECT id, description, due_date, completion_date, is_recurring, recurrence_interval, recurrence_type FROM tasks")) {
            while (logQuery.next()) {
                int id = logQuery.value(0).toInt();
                QString desc = logQuery.value(1).toString();
                QString dueDate = logQuery.value(2).toString();
                QString completionDate = logQuery.value(3).toString();
                int isRecurring = logQuery.value(4).toInt();
                int recurrenceInterval = logQuery.value(5).toInt();
                QString recurrenceType = logQuery.value(6).toString();

                qDebug() << "Task ID:" << id
                         << "Description:" << desc
                         << "Due Date:" << dueDate
                         << "Completion Date:" << completionDate
                         << "Is Recurring:" << isRecurring
                         << "Recurrence Interval:" << recurrenceInterval
                         << "Recurrence Type:" << recurrenceType;
            }
        } else {
            qDebug() << "Error querying tasks table:" << logQuery.lastError();
        }

        // Reload tasks to reflect the new recurring task or completed status
        loadTasks();
    } else {
        qDebug() << "Error marking task as completed: " << query.lastError();
    }
}

void TankProfilePage::deleteTask(const QString &description)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Task", "Are you sure you want to delete this task?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM tasks WHERE description = :description AND tank_id = (SELECT id FROM tanks WHERE name = :name)");
        query.bindValue(":description", description);
        query.bindValue(":name", tankName);

        if (query.exec()) {
            qDebug() << "Task successfully deleted: " << description;
            loadTasks();  // Refresh the task list
        } else {
            qDebug() << "Error deleting task: " << query.lastError();
        }
    }
}

void TankProfilePage::loadTasks()
{
    taskListWidget->clear();
    QSqlQuery query;
    query.prepare("SELECT description, due_date FROM tasks WHERE tank_id = (SELECT id FROM tanks WHERE name = :name) AND completion_date IS NULL ORDER BY due_date ASC");
    query.bindValue(":name", tankName);

    if (query.exec()) {
        while (query.next()) {
            QString description = query.value(0).toString();
            QDate dueDate = QDate::fromString(query.value(1).toString(), "yyyy-MM-dd");

            // Create a TaskItemWidget instead of just a text item
            TaskItemWidget *taskItem = new TaskItemWidget(description, dueDate.toString("yyyy-MM-dd"), this);

            // Connect signals for edit and complete actions
            connect(taskItem, &TaskItemWidget::editClicked, this, &TankProfilePage::editTask);
            connect(taskItem, &TaskItemWidget::completeClicked, this, &TankProfilePage::completeTask);
            connect(taskItem, &TaskItemWidget::deleteClicked, this, &TankProfilePage::deleteTask);


            // Create a QListWidgetItem and set the TaskItemWidget as its widget
            QListWidgetItem *item = new QListWidgetItem(taskListWidget);
            item->setSizeHint(taskItem->sizeHint());
            taskListWidget->setItemWidget(item, taskItem);
        }
    } else {
        qDebug() << "Error loading tasks: " << query.lastError();
    }
}


void TankProfilePage::viewAllActiveTasks()
{
    QDialog dialog(this);
    dialog.setWindowTitle("All Active Tasks for " + tankName);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QListWidget *allTasksListWidget = new QListWidget(&dialog);
    layout->addWidget(allTasksListWidget);

    QSqlQuery query;
    query.prepare("SELECT description, due_date FROM tasks WHERE tank_id = (SELECT id FROM tanks WHERE name = :name) AND completion_date IS NULL ORDER BY due_date ASC");
    query.bindValue(":name", tankName);

    if (query.exec()) {
        while (query.next()) {
            QString description = query.value(0).toString();
            QString dueDate = query.value(1).toString();

            QListWidgetItem *item = new QListWidgetItem(QString("%1 (Due: %2)").arg(description).arg(dueDate), allTasksListWidget);
            allTasksListWidget->addItem(item);
        }
    } else {
        qDebug() << "Error loading active tasks: " << query.lastError();
    }

    QPushButton *closeButton = new QPushButton("Close", &dialog);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeButton);

    dialog.exec();
}




void TankProfilePage::goBackToTankSelection()
{
    emit backToTankSelectionRequested();
}

void TankProfilePage::loadTankData()
{
    // Load tank type and volume
    QSqlQuery query;
    query.prepare("SELECT type, volume FROM tanks WHERE name = :name");
    query.bindValue(":name", tankName);

    if (query.exec() && query.next()) {
        QString tankType = query.value(0).toString();
        double tankVolume = query.value(1).toDouble();

        tankTypeLabel->setText("Tank Type: " + tankType);
        tankVolumeLabel->setText("Tank Volume: " + QString::number(tankVolume) + " gallons");
    } else {
        tankTypeLabel->setText("Tank Type: Unknown");
        tankVolumeLabel->setText("Tank Volume: Unknown");
        qDebug() << "Error loading tank type and volume:" << query.lastError();
    }

    loadParameters();
    loadTasks();
    printLivestockTableSchema();
    loadLivestock();
}

void TankProfilePage::printLivestockTableSchema()
{
    QSqlQuery query;
    if (query.exec("PRAGMA table_info(livestock)")) {
        while (query.next()) {
            QString columnName = query.value(1).toString();
            QString columnType = query.value(2).toString();
            qDebug() << "Column:" << columnName << "Type:" << columnType;
        }
    } else {
        qDebug() << "Failed to retrieve schema:" << query.lastError();
    }
}


void TankProfilePage::loadParameters()
{
    QSqlQuery query;
    query.prepare("SELECT calcium, calcium_last_updated, alkalinity, alkalinity_last_updated, nitrate, nitrate_last_updated, phosphate, phosphate_last_updated FROM parameters WHERE tank_id = (SELECT id FROM tanks WHERE name = :name) ORDER BY date DESC LIMIT 1");
    query.bindValue(":name", tankName);

    if (query.exec()) {
        if (query.next()) {
            QString calcium = query.value(0).isNull() ? "Not set" : QString::number(query.value(0).toDouble());
            QString alkalinity = query.value(2).isNull() ? "Not set" : QString::number(query.value(2).toDouble());
            QString nitrate = query.value(4).isNull() ? "Not set" : QString::number(query.value(4).toDouble());
            QString phosphate = query.value(6).isNull() ? "Not set" : QString::number(query.value(6).toDouble());

            QString parametersText = QString("Calcium: %1\nAlkalinity: %2\nNitrate: %3\nPhosphate: %4")
                                         .arg(calcium)
                                         .arg(alkalinity)
                                         .arg(nitrate)
                                         .arg(phosphate);

            qDebug() << "Loaded parameters: " << parametersText;
            parametersLabel->setText(parametersText);
        } else {
            parametersLabel->setText("No parameters available.");
            qDebug() << "No parameters found for tank: " << tankName;
        }
    } else {
        parametersLabel->setText("No parameters available.");
        qDebug() << "Error loading parameters:" << query.lastError();
    }
}


void TankProfilePage::logNewParameters()
{
    LogParametersDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QMap<QString, double> parameters = dialog.getParameters();

        // Retrieve the tank_id
        QSqlQuery idQuery;
        idQuery.prepare("SELECT id FROM tanks WHERE name = :name");
        idQuery.bindValue(":name", tankName);

        if (!idQuery.exec() || !idQuery.next()) {
            qDebug() << "Error retrieving tank ID:" << idQuery.lastError();
            QMessageBox::critical(this, "Error", "Failed to retrieve tank ID.");
            return;
        }

        int tankId = idQuery.value(0).toInt();
        qDebug() << "Tank ID:" << tankId;

        // Insert or update parameters
        for (auto it = parameters.begin(); it != parameters.end(); ++it) {
            QString param = it.key().toLower();
            qint64 currentDateTime = QDateTime::currentDateTime().toSecsSinceEpoch();

            // Insert new record or update existing record
            QString insertSql = QString("INSERT OR REPLACE INTO parameters (tank_id, date, %1, %1_last_updated) "
                                        "VALUES (:tank_id, :date, :value, :date)").arg(param);

            QSqlQuery query;
            query.prepare(insertSql);
            query.bindValue(":tank_id", tankId);
            query.bindValue(":date", currentDateTime);
            query.bindValue(":value", it.value());

            qDebug() << "Executing SQL:" << insertSql;
            qDebug() << "With tank_id:" << tankId << ", date:" << currentDateTime << ", value:" << it.value();

            if (!query.exec()) {
                qDebug() << "Error inserting/updating parameter: " << param << " - " << query.lastError();
                QMessageBox::critical(this, "Error", "Failed to insert/update parameter.");
                return;
            } else {
                qDebug() << "Successfully inserted/updated parameter: " << param;
            }
        }

        // Log state of parameters table after insert/update
        logCurrentParameters(tankId);

        // Reload parameters to ensure the display is updated
        loadParameters();
    }
}

void TankProfilePage::logCurrentParameters(int tankId)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM parameters WHERE tank_id = :tank_id ORDER BY date DESC");
    query.bindValue(":tank_id", tankId);

    if (query.exec()) {
        qDebug() << "Current parameters for tank ID:" << tankId;
        while (query.next()) {
            QString logString = "Date: " + QDateTime::fromSecsSinceEpoch(query.value("date").toLongLong()).toString() + ", ";
            logString += "pH: " + query.value("ph").toString() + ", ";
            logString += "Temperature: " + query.value("temperature").toString() + ", ";
            logString += "Salinity: " + query.value("salinity").toString() + ", ";
            logString += "Ammonia: " + query.value("ammonia").toString() + ", ";
            logString += "Nitrite: " + query.value("nitrite").toString() + ", ";
            logString += "Nitrate: " + query.value("nitrate").toString() + ", ";
            logString += "Phosphate: " + query.value("phosphate").toString() + ", ";
            logString += "Alkalinity: " + query.value("alkalinity").toString() + ", ";
            logString += "Calcium: " + query.value("calcium").toString() + ", ";
            logString += "Magnesium: " + query.value("magnesium").toString() + ", ";
            logString += "Iodine: " + query.value("iodine").toString() + ", ";
            logString += "Strontium: " + query.value("strontium").toString();
            qDebug() << logString;
        }
    } else {
        qDebug() << "Error querying parameters:" << query.lastError();
    }
}

void TankProfilePage::viewAllParameters()
{
    ViewAllParametersDialog dialog(tankName, this);
    dialog.exec();
}

void TankProfilePage::viewParameterTrends()
{
    TrendsDialog dialog(tankName, this);
    dialog.exec();
}


void TankProfilePage::addLivestock() {
    CreateCreatureDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString species = dialog.getSpecies();
        QString name = dialog.getName();
        double size = dialog.getSize();
        QDate dateAcquired = dialog.getDateAcquired();
        QString notes = dialog.getNotes();

        // Convert the acquired date to a timestamp (seconds since epoch)
        qint64 dateAddedTimestamp = dateAcquired.startOfDay().toSecsSinceEpoch();

        QSqlQuery query;
        query.prepare("INSERT INTO livestock (tank_id, name, species, size, date_added, notes) VALUES "
                      "((SELECT id FROM tanks WHERE name = :tankName), :name, :species, :size, :dateAdded, :notes)");
        query.bindValue(":tankName", tankName);
        query.bindValue(":name", name);
        query.bindValue(":species", species);
        query.bindValue(":size", size);
        query.bindValue(":dateAdded", dateAddedTimestamp);
        query.bindValue(":notes", notes);

        if (query.exec()) {
            qDebug() << "Livestock added: " << name;
            loadLivestock();  // Refresh the livestock list
        } else {
            qDebug() << "Error adding livestock: " << query.lastError();
        }
    }
}

void TankProfilePage::loadLivestock()
{
    livestockListWidget->clear();

    QSqlQuery query;
    query.prepare("SELECT name, species, size, date_added, notes FROM livestock WHERE tank_id = (SELECT id FROM tanks WHERE name = :name)");
    query.bindValue(":name", tankName);

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(0).toString();
            QString species = query.value(1).toString();
            double size = query.value(2).toDouble();
            qint64 dateAddedTimestamp = query.value(3).toLongLong();
            QString notes = query.value(4).toString();

            QString dateAdded = QDateTime::fromSecsSinceEpoch(dateAddedTimestamp).toString("yyyy-MM-dd");
            QString livestockText = QString("%1 (%2), Size: %3 inches, Acquired on: %4")
                                        .arg(name)
                                        .arg(species)
                                        .arg(size)
                                        .arg(dateAdded);

            if (!notes.isEmpty()) {
                livestockText += QString(", Notes: %1").arg(notes);
            }

            // Create a widget to hold the livestock text and buttons
            QWidget *livestockItemWidget = new QWidget(this);
            QHBoxLayout *layout = new QHBoxLayout(livestockItemWidget);

            QLabel *livestockLabel = new QLabel(livestockText, livestockItemWidget);
            layout->addWidget(livestockLabel);

            // View button
            QPushButton *viewButton = new QPushButton("View", livestockItemWidget);
            connect(viewButton, &QPushButton::clicked, [this, name]() {
                viewCreatureProfile(name);
            });

            layout->addWidget(viewButton);

            // Delete button
            QPushButton *deleteButton = new QPushButton("Delete", livestockItemWidget);
            connect(deleteButton, &QPushButton::clicked, [this, name]() {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(this, "Delete Creature", "Are you sure you want to delete " + name + "?",
                                              QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    QSqlQuery deleteQuery;
                    deleteQuery.prepare("DELETE FROM livestock WHERE name = :name AND tank_id = (SELECT id FROM tanks WHERE name = :tankName)");
                    deleteQuery.bindValue(":name", name);
                    deleteQuery.bindValue(":tankName", tankName);

                    if (deleteQuery.exec()) {
                        qDebug() << "Livestock deleted: " << name;
                        loadLivestock();  // Refresh the livestock list
                    } else {
                        qDebug() << "Error deleting livestock: " << deleteQuery.lastError();
                    }
                }
            });
            layout->addWidget(deleteButton);

            // Add the widget to the QListWidget
            QListWidgetItem *item = new QListWidgetItem(livestockListWidget);
            item->setSizeHint(livestockItemWidget->sizeHint());
            livestockListWidget->setItemWidget(item, livestockItemWidget);
        }
    } else {
        qDebug() << "Error loading livestock:" << query.lastError();
    }
}



void TankProfilePage::viewCreatureProfile(const QString &name) {
    qDebug() << "Opening profile for" << name;
    CreatureProfilePage *profilePage = new CreatureProfilePage(name);
    profilePage->show();
}











