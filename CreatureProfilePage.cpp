#include "CreatureProfilePage.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QBuffer>
#include <QDateTime>
#include <QInputDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

CreatureProfilePage::CreatureProfilePage(const QString &creatureName, QWidget *parent)
    : QWidget(parent), creatureName(creatureName)
{
    setupUI();
    loadCreatureProfile();   // Load existing creature profile data
    loadWidgetsFromDatabase(); // Load custom widgets from the database
}

void CreatureProfilePage::setupUI()
{
    layout = new QVBoxLayout(this);

    creatureNameLabel = new QLabel(creatureName, this);
    layout->addWidget(creatureNameLabel);

    speciesField = new QLineEdit(this);
    speciesField->setPlaceholderText("Species");
    layout->addWidget(speciesField);

    sizeField = new QLineEdit(this);
    sizeField->setPlaceholderText("Size");
    layout->addWidget(sizeField);

    dateAddedField = new QLineEdit(this);
    dateAddedField->setPlaceholderText("Date Added (yyyy-MM-dd)");
    layout->addWidget(dateAddedField);

    notesField = new QTextEdit(this);
    notesField->setPlaceholderText("Notes");
    layout->addWidget(notesField);

    saveButton = new QPushButton("Save", this);
    connect(saveButton, &QPushButton::clicked, this, &CreatureProfilePage::onSaveButtonClicked);
    layout->addWidget(saveButton);

    // Create a menu bar for adding widgets
    createMenuBar();

    setLayout(layout);
}

void CreatureProfilePage::createMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *addWidgetMenu = new QMenu("Add Widget", menuBar);
    menuBar->addMenu(addWidgetMenu);

    QAction *addImageAction = new QAction("Add Image", addWidgetMenu);
    connect(addImageAction, &QAction::triggered, this, &CreatureProfilePage::addImageWidget);
    addWidgetMenu->addAction(addImageAction);

    QAction *addTextAction = new QAction("Add Text", addWidgetMenu);
    connect(addTextAction, &QAction::triggered, this, &CreatureProfilePage::addTextWidget);
    addWidgetMenu->addAction(addTextAction);

    QAction *addTimelineAction = new QAction("Add Timeline", addWidgetMenu);
    connect(addTimelineAction, &QAction::triggered, this, &CreatureProfilePage::addTimelineWidget);
    addWidgetMenu->addAction(addTimelineAction);

    layout->setMenuBar(menuBar);
}

void CreatureProfilePage::addImageWidget()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty()) {
        QPixmap image(filePath);
        QLabel *imageLabel = new QLabel(this);
        imageLabel->setPixmap(image);
        layout->addWidget(imageLabel);
        customWidgets.append(imageLabel);  // Track custom widget
        saveWidgetToDatabase(imageLabel, 1);  // Save image widget to database
    }
}

void CreatureProfilePage::addTextWidget()
{
    QTextEdit *textEdit = new QTextEdit(this);
    layout->addWidget(textEdit);
    customWidgets.append(textEdit);  // Track custom widget
    saveWidgetToDatabase(textEdit, 2);  // Save text widget to database
}

void CreatureProfilePage::addTimelineWidget()
{
    QWidget *timelineContainer = new QWidget(this);
    QVBoxLayout *timelineLayout = new QVBoxLayout(timelineContainer);

    QListWidget *timelineWidget = new QListWidget(this);
    timelineWidget->setDragDropMode(QAbstractItemView::InternalMove);

    QPushButton *addEventButton = new QPushButton("Add Event", timelineContainer);
    connect(addEventButton, &QPushButton::clicked, [=]() {
        bool ok;
        QString eventText = QInputDialog::getText(this, "Add Event", "Event:", QLineEdit::Normal, "", &ok);
        if (ok && !eventText.isEmpty()) {
            QListWidgetItem *newItem = new QListWidgetItem(eventText);
            timelineWidget->addItem(newItem);
        }
    });

    timelineLayout->addWidget(timelineWidget);
    timelineLayout->addWidget(addEventButton);

    layout->addWidget(timelineContainer);
    customWidgets.append(timelineContainer);  // Track custom widget
    saveWidgetToDatabase(timelineContainer, 3);  // Save timeline widget to database
}

void CreatureProfilePage::deleteWidget()
{
    if (!customWidgets.isEmpty()) {
        QWidget *widgetToRemove = customWidgets.takeLast();  // Get the last added custom widget
        layout->removeWidget(widgetToRemove);
        delete widgetToRemove;
    }
}

void CreatureProfilePage::onSaveButtonClicked()
{
    saveCreatureProfile();
}

void CreatureProfilePage::saveCreatureProfile()
{
    QSqlQuery query;
    query.prepare("UPDATE livestock SET species = ?, size = ?, date_added = ?, notes = ? WHERE name = ?");
    query.addBindValue(speciesField->text());
    query.addBindValue(sizeField->text());

    // Convert QDate to QDateTime and then to timestamp
    QDate dateAdded = QDate::fromString(dateAddedField->text(), "yyyy-MM-dd");
    QDateTime dateTimeAdded(dateAdded, QTime(0, 0));
    qint64 dateAddedTimestamp = dateTimeAdded.toSecsSinceEpoch();
    query.addBindValue(dateAddedTimestamp);

    query.addBindValue(notesField->toPlainText());
    query.addBindValue(creatureName);

    if (!query.exec()) {
        qDebug() << "Error saving creature profile:" << query.lastError();
    } else {
        qDebug() << "Profile saved for" << creatureName;
    }
}


void CreatureProfilePage::loadCreatureProfile()
{
    QSqlQuery query;
    query.prepare("SELECT species, size, date_added, notes FROM livestock WHERE name = ?");
    query.addBindValue(creatureName);

    if (query.exec()) {
        if (query.next()) {
            speciesField->setText(query.value(0).toString());
            sizeField->setText(query.value(1).toString());

            qint64 dateAddedTimestamp = query.value(2).toLongLong();
            QString dateAddedString = QDateTime::fromSecsSinceEpoch(dateAddedTimestamp).toString("yyyy-MM-dd");
            dateAddedField->setText(dateAddedString);

            notesField->setPlainText(query.value(3).toString());
            qDebug() << "Profile loaded for" << creatureName;
        } else {
            qDebug() << "No creature found with name:" << creatureName;
        }
    } else {
        qDebug() << "Error loading creature profile:" << query.lastError();
    }
}

void CreatureProfilePage::saveWidgetToDatabase(QWidget *widget, int widgetType)
{
    QSqlQuery query;
    query.prepare("INSERT INTO CreatureWidgets (creatureName, widgetType, widgetData) VALUES (?, ?, ?)");

    query.addBindValue(creatureName);
    query.addBindValue(widgetType);

    QByteArray widgetData;
    QBuffer buffer(&widgetData);
    buffer.open(QIODevice::WriteOnly);

    if (widgetType == 1) {
        QLabel *label = qobject_cast<QLabel*>(widget);
        if (label && !label->pixmap().isNull()) {
            label->pixmap().save(&buffer, "PNG");  // Save pixmap data
        }
    } else if (widgetType == 2) {
        QTextEdit *textEdit = qobject_cast<QTextEdit*>(widget);
        if (textEdit) {
            widgetData = textEdit->toPlainText().toUtf8();  // Save text data
        }
    } else if (widgetType == 3) {
        QListWidget *listWidget = timelineWidget; // Use the class-level timelineWidget pointer
        if (listWidget) {
            QStringList items;
            for (int i = 0; i < listWidget->count(); ++i) {
                items << listWidget->item(i)->text();
            }
            widgetData = items.join('\n').toUtf8();  // Save timeline data
        }
    }

    query.addBindValue(widgetData);

    if (!query.exec()) {
        qDebug() << "Error saving widget to database:" << query.lastError();
    }
}

void CreatureProfilePage::loadWidgetsFromDatabase()
{
    QSqlQuery query;
    query.prepare("SELECT widgetType, widgetData FROM CreatureWidgets WHERE creatureName = ?");
    query.addBindValue(creatureName);

    if (query.exec()) {
        while (query.next()) {
            int widgetType = query.value(0).toInt();
            QByteArray widgetData = query.value(1).toByteArray();

            if (widgetType == 1) {  // Image widget
                QPixmap image;
                image.loadFromData(widgetData, "PNG");
                QLabel *imageLabel = new QLabel(this);
                imageLabel->setPixmap(image);
                layout->addWidget(imageLabel);
                customWidgets.append(imageLabel);
            } else if (widgetType == 2) {  // Text widget
                QTextEdit *textEdit = new QTextEdit(this);
                textEdit->setPlainText(QString::fromUtf8(widgetData));
                layout->addWidget(textEdit);
                customWidgets.append(textEdit);
            } else if (widgetType == 3) {  // Timeline widget
                QWidget *timelineContainer = new QWidget(this);
                QVBoxLayout *timelineLayout = new QVBoxLayout(timelineContainer);

                QListWidget *listWidget = new QListWidget(this);
                listWidget->setDragDropMode(QAbstractItemView::InternalMove);

                QStringList items = QString::fromUtf8(widgetData).split('\n');
                listWidget->addItems(items);

                QPushButton *addEventButton = new QPushButton("Add Event", timelineContainer);
                connect(addEventButton, &QPushButton::clicked, [=]() {
                    bool ok;
                    QString eventText = QInputDialog::getText(this, "Add Event", "Event:", QLineEdit::Normal, "", &ok);
                    if (ok && !eventText.isEmpty()) {
                        QListWidgetItem *newItem = new QListWidgetItem(eventText);
                        listWidget->addItem(newItem);
                    }
                });

                timelineLayout->addWidget(listWidget);
                timelineLayout->addWidget(addEventButton);

                layout->addWidget(timelineContainer);
                customWidgets.append(timelineContainer);
            }
        }
    } else {
        qDebug() << "Error loading widgets from database:" << query.lastError();
    }
}
