#include "CreateCreatureDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

CreateCreatureDialog::CreateCreatureDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Species
    mainLayout->addWidget(new QLabel("Species:", this));
    speciesEdit = new QLineEdit(this);
    mainLayout->addWidget(speciesEdit);

    // Name
    mainLayout->addWidget(new QLabel("Name:", this));
    nameEdit = new QLineEdit(this);
    mainLayout->addWidget(nameEdit);

    // Size
    mainLayout->addWidget(new QLabel("Size (in inches):", this));
    sizeSpinBox = new QDoubleSpinBox(this);
    sizeSpinBox->setRange(0.1, 100.0);
    sizeSpinBox->setSingleStep(0.1);
    mainLayout->addWidget(sizeSpinBox);

    // Date Acquired
    mainLayout->addWidget(new QLabel("Date Acquired:", this));
    dateAcquiredEdit = new QDateEdit(QDate::currentDate(), this);
    dateAcquiredEdit->setCalendarPopup(true);
    mainLayout->addWidget(dateAcquiredEdit);

    // Notes
    mainLayout->addWidget(new QLabel("Notes:", this));
    notesEdit = new QTextEdit(this);
    mainLayout->addWidget(notesEdit);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &CreateCreatureDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &CreateCreatureDialog::reject);
}

QString CreateCreatureDialog::getSpecies() const {
    return speciesEdit->text();
}

QString CreateCreatureDialog::getName() const {
    return nameEdit->text();
}

double CreateCreatureDialog::getSize() const {
    return sizeSpinBox->value();
}

QDate CreateCreatureDialog::getDateAcquired() const {
    return dateAcquiredEdit->date();
}

QString CreateCreatureDialog::getNotes() const {
    return notesEdit->toPlainText();
}

void CreateCreatureDialog::onAccept() {
    if (speciesEdit->text().isEmpty() || nameEdit->text().isEmpty()) {
        // Optionally, add error handling here
        return;
    }
    accept();
}
