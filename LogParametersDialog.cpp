#include "LogParametersDialog.h"

LogParametersDialog::LogParametersDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *instructions = new QLabel("Select a parameter to add:", this);
    layout->addWidget(instructions);

    parametersComboBox = new QComboBox(this);
    parametersComboBox->addItems({"pH", "Temperature", "Salinity", "Ammonia", "Nitrite", "Nitrate", "Phosphate", "Alkalinity", "Calcium", "Magnesium", "Iodine", "Strontium"});
    layout->addWidget(parametersComboBox);

    addButton = new QPushButton("Add Parameter", this);
    connect(addButton, &QPushButton::clicked, this, &LogParametersDialog::addParameter);
    layout->addWidget(addButton);

    parametersLayout = new QVBoxLayout();
    layout->addLayout(parametersLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    logButton = new QPushButton("Log Parameters", this);
    cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(logButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(logButton, &QPushButton::clicked, this, &LogParametersDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &LogParametersDialog::reject);
}

void LogParametersDialog::addParameter()
{
    QString parameter = parametersComboBox->currentText();
    if (parametersMap.contains(parameter)) {
        return; // Parameter already added
    }

    QLabel *label = new QLabel(parameter + ":", this);
    QLineEdit *lineEdit = new QLineEdit(this);
    parametersMap[parameter] = lineEdit;

    QHBoxLayout *parameterLayout = new QHBoxLayout();
    parameterLayout->addWidget(label);
    parameterLayout->addWidget(lineEdit);
    parametersLayout->addLayout(parameterLayout);
}

QMap<QString, double> LogParametersDialog::getParameters() const
{
    QMap<QString, double> parameters;
    for (auto it = parametersMap.begin(); it != parametersMap.end(); ++it) {
        parameters[it.key()] = it.value()->text().toDouble();
    }
    return parameters;
}
