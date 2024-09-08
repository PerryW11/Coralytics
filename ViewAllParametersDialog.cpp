#include "ViewAllParametersDialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDateTime>

ViewAllParametersDialog::ViewAllParametersDialog(const QString &tankName, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("All Parameters");

    // Set the initial size of the dialog
    resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(this);

    parametersTable = new QTableWidget(this);
    parametersTable->setColumnCount(3); // Add a column for the last updated date
    parametersTable->setHorizontalHeaderLabels(QStringList() << "Parameter" << "Value" << "Last Updated");
    parametersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(parametersTable);

    loadAllParameters(tankName);

    // Clear selection and focus
    parametersTable->clearSelection();
    parametersTable->setFocusPolicy(Qt::NoFocus);
}

void ViewAllParametersDialog::loadAllParameters(const QString &tankName)
{
    QSqlQuery query;
    query.prepare(
        "SELECT calcium, calcium_last_updated, alkalinity, alkalinity_last_updated, "
        "nitrate, nitrate_last_updated, phosphate, phosphate_last_updated, "
        "ph, ph_last_updated, temperature, temperature_last_updated, "
        "salinity, salinity_last_updated, ammonia, ammonia_last_updated, "
        "nitrite, nitrite_last_updated, magnesium, magnesium_last_updated, "
        "iodine, iodine_last_updated, strontium, strontium_last_updated "
        "FROM parameters WHERE tank_id = (SELECT id FROM tanks WHERE name = :name) "
        "ORDER BY date DESC LIMIT 1");

    query.bindValue(":name", tankName);

    if (query.exec()) {
        if (query.next()) {
            int row = 0;
            QStringList parameters = {"Calcium", "Alkalinity", "Nitrate", "Phosphate", "pH",
                                      "Temperature", "Salinity", "Ammonia", "Nitrite",
                                      "Magnesium", "Iodine", "Strontium"};

            for (int i = 0; i < parameters.size(); ++i) {
                QString parameterName = parameters[i];
                QString value = query.value(i * 2).isNull() ? "Not set" : QString::number(query.value(i * 2).toDouble());
                QString lastUpdated = query.value(i * 2 + 1).isNull() ? "Never" : QDateTime::fromSecsSinceEpoch(query.value(i * 2 + 1).toLongLong()).toString("MMM dd, yyyy");

                // Insert a new row in the table for each parameter
                parametersTable->insertRow(row);
                parametersTable->setItem(row, 0, new QTableWidgetItem(parameterName));
                parametersTable->setItem(row, 1, new QTableWidgetItem(value));
                parametersTable->setItem(row, 2, new QTableWidgetItem(lastUpdated));
                row++;
            }
        } else {
            qDebug() << "No parameters found for tank:" << tankName;
        }
    } else {
        qDebug() << "Error executing query:" << query.lastError();
    }
}




