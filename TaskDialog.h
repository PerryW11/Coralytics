#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QSpinBox>

class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDialog(QWidget *parent = nullptr);

    QString getDescription() const;
    QDate getDueDate() const;
    int getRecurrenceInterval() const;
    QString getRecurrenceType() const;

    void setDescription(const QString &description);
    void setDueDate(const QDate &dueDate);
    void setRecurrenceInterval(int interval);
    void setRecurrenceType(const QString &type);

private:
    QLineEdit *descriptionEdit;
    QDateEdit *dueDateEdit;
    QSpinBox *recurrenceIntervalSpinBox;
    QComboBox *recurrenceTypeComboBox;
};

#endif // TASKDIALOG_H
