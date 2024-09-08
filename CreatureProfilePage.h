#ifndef CREATUREPROFILEPAGE_H
#define CREATUREPROFILEPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPixmap>
#include <QListWidget>

class CreatureProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit CreatureProfilePage(const QString &creatureName, QWidget *parent = nullptr);
    void loadCreatureProfile();
    void saveCreatureProfile();
    void addCustomWidget(const QString &widgetType);

private slots:
    void addImageWidget();
    void addTextWidget();
    void addTimelineWidget();
    void deleteWidget();
    void onSaveButtonClicked();

private:
    QString creatureName;
    QVBoxLayout *layout;
    QLabel *creatureNameLabel;
    QLineEdit *speciesField;
    QLineEdit *sizeField;
    QLineEdit *dateAddedField;
    QTextEdit *notesField;
    QPushButton *saveButton;

    QList<QWidget*> customWidgets;  // List to keep track of custom widgets

    QListWidget *timelineWidget; // Pointer to timeline widget for saving/loading

    void setupUI();
    void createMenuBar();
    void saveWidgetToDatabase(QWidget *widget, int widgetType);
    void loadWidgetsFromDatabase();
};

#endif // CREATUREPROFILEPAGE_H
