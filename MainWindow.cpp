#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "TankProfilePage.h"
#include <QInputDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    initializeDatabase();
    resetDatabase();
    ui->setupUi(this);

    // Ensure central widget and layout are valid
    if (!ui->centralwidget || !ui->centralwidget->layout()) {
        qDebug() << "Error: central widget or its layout is not valid. Creating a new layout.";
        QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);
        ui->centralwidget->setLayout(layout);
    }

    // Create a QLabel for the welcome message and add it to the layout
    QLabel *welcomeLabel = new QLabel("Welcome to Coralytics!", this);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(16);
    welcomeFont.setBold(true);
    welcomeLabel->setFont(welcomeFont);
    welcomeLabel->setContentsMargins(0, 10, 10, 10);
    ui->centralwidget->layout()->addWidget(welcomeLabel);

    // Create a button for creating a new tank profile and add it to the layout
    QPushButton *createTankButton = new QPushButton("Create New Tank Profile", this);
    createTankButton->setFixedWidth(200); // Set a fixed width for consistency
    createTankButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // Make the button size fixed
    connect(createTankButton, &QPushButton::clicked, this, &MainWindow::addNewTank);
    ui->centralwidget->layout()->addWidget(createTankButton);

    // Create a QLabel for the existing tank profiles section title and add it to the layout
    QLabel *existingTanksLabel = new QLabel("Current Tank Profiles", this);
    QFont sectionFont = existingTanksLabel->font();
    // sectionFont.setPointSize(10);
    sectionFont.setBold(true);
    existingTanksLabel->setFont(sectionFont);
    existingTanksLabel->setContentsMargins(0, 10, 10, 10);
    ui->centralwidget->layout()->addWidget(existingTanksLabel);

    // Create the tank list widget programmatically and add it to the layout
    tankListWidget = new QListWidget(this);
    ui->centralwidget->layout()->addWidget(tankListWidget);

    // Enable custom context menu
    tankListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tankListWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    // Connect signals and slots for the programmatically created elements
    connect(tankListWidget, &QListWidget::itemClicked, this, &MainWindow::selectTank);
    // Load tank profiles and refresh the list
    loadTankProfiles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("coralytics.db");

    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
    } else {
        qDebug() << "Database: connection ok";
        QSqlQuery query;
        // Create tanks table if it doesn't exist
        query.exec("CREATE TABLE IF NOT EXISTS tanks ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "name TEXT UNIQUE, "
                   "type TEXT, "
                   "volume REAL)");

        // Create parameters table if it doesn't exist
        if (!query.exec("CREATE TABLE IF NOT EXISTS parameters ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "tank_id INTEGER NOT NULL, "
                        "date INTEGER NOT NULL, "
                        "ph REAL, ph_last_updated INTEGER, "
                        "temperature REAL, temperature_last_updated INTEGER, "
                        "salinity REAL, salinity_last_updated INTEGER, "
                        "ammonia REAL, ammonia_last_updated INTEGER, "
                        "nitrite REAL, nitrite_last_updated INTEGER, "
                        "nitrate REAL, nitrate_last_updated INTEGER, "
                        "phosphate REAL, phosphate_last_updated INTEGER, "
                        "alkalinity REAL, alkalinity_last_updated INTEGER, "
                        "calcium REAL, calcium_last_updated INTEGER, "
                        "magnesium REAL, magnesium_last_updated INTEGER, "
                        "iodine REAL, iodine_last_updated INTEGER, "
                        "strontium REAL, strontium_last_updated INTEGER, "
                        "FOREIGN KEY (tank_id) REFERENCES tanks(id))")) {
            qDebug() << "Error creating parameters table:" << query.lastError();
        }

        // Create tasks table if it doesn't exist
        query.exec("CREATE TABLE IF NOT EXISTS tasks ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "tank_id INTEGER, "
                   "description TEXT, "
                   "due_date TEXT, "
                   "completion_date TEXT, "
                   "is_recurring INTEGER, "
                   "recurrence_interval INTEGER, "
                   "recurrence_type TEXT, "
                   "FOREIGN KEY(tank_id) REFERENCES tanks(id))");

        query.exec("CREATE TABLE IF NOT EXISTS livestock ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "tank_id INTEGER, "
                   "name TEXT NOT NULL, "
                   "species TEXT, "
                   "size REAL, "
                   "date_added INTEGER, "
                   "notes TEXT, "
                   "UNIQUE(name), "
                   "FOREIGN KEY(tank_id) REFERENCES tanks(id))");

        // Create CreatureWidgets table if it doesn't exist
        query.exec("CREATE TABLE IF NOT EXISTS CreatureWidgets ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "creatureName TEXT, "
                   "widgetType INTEGER, "
                   "widgetData BLOB, "
                   "FOREIGN KEY(creatureName) REFERENCES livestock(name))");

        // Debugging: Log table creation
        if (query.lastError().isValid()) {
            qDebug() << "Error creating tables:" << query.lastError();
        } else {
            qDebug() << "Tables created/verified successfully.";
        }
    }
}


void MainWindow::resetDatabase()
{
    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
        return;
    }

    QSqlQuery query;
    query.exec("DROP TABLE IF EXISTS parameters");
    query.exec("DROP TABLE IF EXISTS livestock");
    query.exec("DROP TABLE IF EXISTS tasks");
    query.exec("DROP TABLE IF EXISTS tanks");
    query.exec("DROP TABLE IF EXISTS CreatureWidgets");

    initializeDatabase();
}


void MainWindow::loadTankProfiles()
{
    // Clear the tank list widget to avoid duplication
    tankListWidget->clear();

    // Query to get existing tank profiles
    QSqlQuery query("SELECT name FROM tanks");
    while (query.next()) {
        QString tankName = query.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(tankName);
        tankListWidget->addItem(item);
    }
}

void MainWindow::refreshTankList()
{
    loadTankProfiles();
}

void MainWindow::addNewTank()
{
    bool ok;
    QString tankName = QInputDialog::getText(this, tr("Add New Tank"),
                                             tr("Tank Name:"), QLineEdit::Normal,
                                             "", &ok);
    if (ok && !tankName.isEmpty()) {
        QStringList tankTypes = {"Freshwater", "Saltwater", "Reef", "Brackish", "Planted", "Breeding"};
        QString tankType = QInputDialog::getItem(this, tr("Select Tank Type"),
                                                 tr("Tank Type:"), tankTypes, 0, false, &ok);
        if (ok && !tankType.isEmpty()) {
            double tankVolume = QInputDialog::getDouble(this, tr("Tank Volume"),
                                                        tr("Enter the volume of the tank (in gallons):"), 0, 0, 10000, 1, &ok);
            if (ok) {
                QSqlQuery query;
                query.prepare("INSERT INTO tanks (name, type, volume) VALUES (:name, :type, :volume)");
                query.bindValue(":name", tankName);
                query.bindValue(":type", tankType);
                query.bindValue(":volume", tankVolume);
                if (!query.exec()) {
                    qDebug() << "Error inserting into table: " << query.lastError();
                } else {
                    refreshTankList();
                }
            }
        }
    }
}


void MainWindow::selectTank(QListWidgetItem *item)
{
    QString selectedTank = item->text();
    qDebug() << "Selected tank:" << selectedTank;

    // Create and display the TankProfilePage
    TankProfilePage *tankProfilePage = new TankProfilePage(selectedTank, this);
    setCentralWidget(tankProfilePage);

    // Connect the signal to the slot
    connect(tankProfilePage, &TankProfilePage::backToTankSelectionRequested, this, &MainWindow::goBackToTankSelection);
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    // Get the item at the clicked position
    rightClickedItem = tankListWidget->itemAt(pos);
    if (rightClickedItem && rightClickedItem->text() != "Add New Tank") {
        QMenu contextMenu(tr("Context menu"), this);

        QAction actionDelete("Delete Tank", this);
        connect(&actionDelete, &QAction::triggered, this, &MainWindow::deleteTank);
        contextMenu.addAction(&actionDelete);

        contextMenu.exec(tankListWidget->mapToGlobal(pos));
    }
}

void MainWindow::deleteTank()
{
    if (rightClickedItem) {
        QString tankName = rightClickedItem->text();
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Tank",
                                      "Are you sure you want to delete the tank profile \"" + tankName + "\"? This action cannot be undone.",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QSqlQuery query;
            query.prepare("DELETE FROM tanks WHERE name = :name");
            query.bindValue(":name", tankName);
            if (!query.exec()) {
                qDebug() << "Error deleting from table: " << query.lastError();
            } else {
                refreshTankList();
            }
        }
    }
}

void MainWindow::goBackToTankSelection()
{
    // Reset the central widget to show the main tank selection screen
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Rebuild the layout and controls in the central widget
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Welcome label
    QLabel *welcomeLabel = new QLabel("Welcome to Coralytics!", this);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(16);
    welcomeFont.setBold(true);
    welcomeLabel->setFont(welcomeFont);
    welcomeLabel->setContentsMargins(0, 10, 10, 10);
    layout->addWidget(welcomeLabel);

    // Create New Tank Profile button
    QPushButton *createTankButton = new QPushButton("Create New Tank Profile", this);
    createTankButton->setFixedWidth(200); // Set a fixed width for consistency
    createTankButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // Make the button size fixed
    connect(createTankButton, &QPushButton::clicked, this, &MainWindow::addNewTank);
    layout->addWidget(createTankButton);

    // Current Tank Profiles label
    QLabel *existingTanksLabel = new QLabel("Current Tank Profiles", this);
    QFont sectionFont = existingTanksLabel->font();
    sectionFont.setBold(true);
    existingTanksLabel->setFont(sectionFont);
    existingTanksLabel->setContentsMargins(0, 10, 10, 10);
    layout->addWidget(existingTanksLabel);

    // Tank list widget
    tankListWidget = new QListWidget(this);
    layout->addWidget(tankListWidget);

    // Enable custom context menu for the tank list widget
    tankListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tankListWidget, &QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);

    // Connect the tank list widget item click signal to the slot
    connect(tankListWidget, &QListWidget::itemClicked, this, &MainWindow::selectTank);

    // Load and refresh the tank profiles
    loadTankProfiles();
}


void MainWindow::showTanks() {
    // Implement the logic to show tanks
}

void MainWindow::showLogParameters() {
    // Implement the logic to log parameters
}

void MainWindow::showViewTrends() {
    // Implement the logic to view trends
}

void MainWindow::showTasks() {
    // Implement the logic to show tasks
}

void MainWindow::showSettings() {
    // Implement the logic to show settings
}
