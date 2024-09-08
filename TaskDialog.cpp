#include "TaskDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QSpinBox>

TaskDialog::TaskDialog(QWidget *parent)
    : QDialog(parent), recurrenceIntervalSpinBox(new QSpinBox(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Task Description
    QLabel *descriptionLabel = new QLabel("Task Description:", this);
    layout->addWidget(descriptionLabel);

    descriptionEdit = new QLineEdit(this);
    layout->addWidget(descriptionEdit);

    // Set focus to the description edit box
    descriptionEdit->setFocus();

    // Due Date
    QLabel *dueDateLabel = new QLabel("Due Date:", this);
    layout->addWidget(dueDateLabel);

    dueDateEdit = new QDateEdit(QDate::currentDate(), this);
    dueDateEdit->setCalendarPopup(true);
    layout->addWidget(dueDateEdit);

    // Recurrence Interval
    QLabel *recurrenceIntervalLabel = new QLabel("Recurrence Interval (Days/Weeks/Months):", this);
    layout->addWidget(recurrenceIntervalLabel);

    recurrenceIntervalSpinBox->setRange(1, 365);  // Example range: 1 to 365
    layout->addWidget(recurrenceIntervalSpinBox);

    // Recurrence Type
    QLabel *recurrenceTypeLabel = new QLabel("Recurrence Type:", this);
    layout->addWidget(recurrenceTypeLabel);

    recurrenceTypeComboBox = new QComboBox(this);
    recurrenceTypeComboBox->addItems({"None", "Daily", "Weekly", "Monthly"});
    layout->addWidget(recurrenceTypeComboBox);

    // Dialog Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);

    setLayout(layout);
}

QString TaskDialog::getDescription() const
{
    return descriptionEdit->text();
}

QDate TaskDialog::getDueDate() const
{
    return dueDateEdit->date();
}

int TaskDialog::getRecurrenceInterval() const
{
    return recurrenceIntervalSpinBox->value();
}

QString TaskDialog::getRecurrenceType() const
{
    return recurrenceTypeComboBox->currentText();
}

void TaskDialog::setDescription(const QString &description)
{
    descriptionEdit->setText(description);
}

void TaskDialog::setDueDate(const QDate &dueDate)
{
    dueDateEdit->setDate(dueDate);
}

void TaskDialog::setRecurrenceInterval(int interval)
{
    recurrenceIntervalSpinBox->setValue(interval);
}

void TaskDialog::setRecurrenceType(const QString &type)
{
    int index = recurrenceTypeComboBox->findText(type);
    if (index != -1) {
        recurrenceTypeComboBox->setCurrentIndex(index);
    }
}
