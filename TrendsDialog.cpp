#include "TrendsDialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QDebug>

TrendsDialog::TrendsDialog(const QString &tankName, QWidget *parent)
    : QDialog(parent), tankName(tankName)
{
    setWindowTitle("View Parameter Trends");
    resize(750, 750);

    layout = new QVBoxLayout(this);

    QHBoxLayout *dropdownLayout = new QHBoxLayout();

    QLabel *parameterLabel = new QLabel("Select Parameter:", this);
    dropdownLayout->addWidget(parameterLabel);

    parameterComboBox = new QComboBox(this);
    // Populate the parameterComboBox based on available data
    populateParameters();
    dropdownLayout->addWidget(parameterComboBox);

    QLabel *timeFrameLabel = new QLabel("Select Time Frame:", this);
    dropdownLayout->addWidget(timeFrameLabel);

    timeFrameComboBox = new QComboBox(this);
    timeFrameComboBox->addItems({"1 week", "2 weeks", "1 month", "3 months", "6 months", "1 year"});
    dropdownLayout->addWidget(timeFrameComboBox);

    layout->addLayout(dropdownLayout);

    connect(parameterComboBox, &QComboBox::currentTextChanged, this, &TrendsDialog::onParameterChanged);
    connect(timeFrameComboBox, &QComboBox::currentTextChanged, this, &TrendsDialog::onTimeFrameChanged);

    generateGraph();
}

TrendsDialog::~TrendsDialog() {}

void TrendsDialog::populateParameters()
{
    QStringList parameters = {"pH", "Temperature", "Salinity", "Ammonia", "Nitrite", "Nitrate", "Phosphate", "Alkalinity", "Calcium", "Magnesium", "Iodine", "Strontium"};

    for (const QString &parameter : parameters) {
        if (parameterHasData(parameter)) {
            parameterComboBox->addItem(parameter);
        }
    }
}

bool TrendsDialog::parameterHasData(const QString &parameter)
{
    int tankId = getTankIdByName(tankName);

    if (tankId == -1) {
        qDebug() << "Invalid tank ID for tank name:" << tankName;
        return false;
    }

    QString parameterColumn = parameter.toLower();
    QString queryString = QString("SELECT COUNT(*) FROM parameters WHERE tank_id = :tank_id AND %1 IS NOT NULL").arg(parameterColumn);

    QSqlQuery query;
    query.prepare(queryString);
    query.bindValue(":tank_id", tankId);

    if (!query.exec()) {
        qDebug() << "Failed to execute query for parameter check:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        int count = query.value(0).toInt();
        return count > 0;
    }

    return false;
}

void TrendsDialog::onParameterChanged()
{
    generateGraph();
}

void TrendsDialog::onTimeFrameChanged()
{
    generateGraph();
}

void TrendsDialog::generateGraph()
{
    while (layout->count() > 1) {  // Start at index 1 to keep the dropdowns
        QLayoutItem *item = layout->takeAt(1);
        if (item) {
            QWidget *widget = item->widget();
            if (widget) {
                widget->setParent(nullptr);
                delete widget;
            }
            delete item;
        }
    }

    QString selectedParameter = parameterComboBox->currentText();
    QString timeFrame = timeFrameComboBox->currentText();

    QDate endDate = QDate::currentDate();
    QDate startDate;

    if (timeFrame == "1 week") {
        startDate = endDate.addDays(-7);  // 7 days including today
    } else if (timeFrame == "2 weeks") {
        startDate = endDate.addDays(-14); // 14 days including today
    } else if (timeFrame == "1 month") {
        startDate = endDate.addMonths(-1);
    } else if (timeFrame == "3 months") {
        startDate = endDate.addMonths(-3);
    } else if (timeFrame == "6 months") {
        startDate = endDate.addMonths(-6);
    } else if (timeFrame == "1 year") {
        startDate = endDate.addYears(-1);
    }

    int tankId = getTankIdByName(tankName);

    if (tankId == -1) {
        qDebug() << "Invalid tank ID for tank name:" << tankName;
        return;
    }

    // Convert the start and end dates to Unix timestamps
    QDateTime startDateTime(startDate, QTime(0, 0, 0));
    QDateTime endDateTime(endDate, QTime(23, 59, 59));
    qint64 startTimestamp = startDateTime.toSecsSinceEpoch();
    qint64 endTimestamp = endDateTime.toSecsSinceEpoch();

    QString parameterColumn = selectedParameter.toLower();
    QString queryString = QString("SELECT date, %1 FROM parameters WHERE tank_id = :tank_id AND date BETWEEN :start_date AND :end_date ORDER BY date ASC")
                              .arg(parameterColumn);

    QSqlQuery query;
    query.prepare(queryString);
    query.bindValue(":tank_id", tankId);
    query.bindValue(":start_date", startTimestamp);
    query.bindValue(":end_date", endTimestamp);

    qDebug() << "Executing SQL:" << queryString;
    qDebug() << "With tank_id:" << tankId << ", start_date:" << startTimestamp << ", end_date:" << endTimestamp;

    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return;
    }

    QLineSeries *series = new QLineSeries();
    series->setName(selectedParameter);

    bool dataFound = false;

    while (query.next()) {
        qint64 timestamp = query.value(0).toLongLong();
        QVariant valueVariant = query.value(1);

        if (valueVariant.isNull()) {
            continue;
        }

        double value = valueVariant.toDouble();
        series->append(timestamp * 1000, value); // Convert to milliseconds for QDateTimeAxis
        dataFound = true;
        QDateTime dateTime;
        dateTime.setSecsSinceEpoch(timestamp);
        qDebug() << "Date:" << dateTime.toString() << "Value:" << value;
    }

    if (!dataFound) {
        qDebug() << "No data found for the selected parameter and timeframe.";
        return;
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("Trends for %1 over %2").arg(selectedParameter).arg(timeFrame));

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat(getDateFormatForTimeFrame(timeFrame));
    axisX->setTitleText("Date");
    axisX->setTickCount(getTickCountForTimeFrame(timeFrame));
    axisX->setMin(QDateTime(startDate, QTime(0, 0, 0)));
    axisX->setMax(QDateTime(endDate, QTime(23, 59, 59)));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText(selectedParameter);
    setAxisRangeAndTicksForParameter(selectedParameter, axisY);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(chartView);
}

int TrendsDialog::getTickCountForTimeFrame(const QString &timeFrame)
{
    if (timeFrame == "1 week") {
        return 7;
    } else if (timeFrame == "2 weeks") {
        return 7;
    } else if (timeFrame == "1 month") {
        return 10;
    } else if (timeFrame == "3 months") {
        return 10;
    } else if (timeFrame == "6 months") {
        return 6;
    } else if (timeFrame == "1 year") {
        return 12;
    } else {
        return 10;
    }
}

QString TrendsDialog::getDateFormatForTimeFrame(const QString &timeFrame)
{
    if (timeFrame == "1 week" || timeFrame == "2 weeks" || timeFrame == "1 month") {
        return "MMM d";
    } else if (timeFrame == "3 months" || timeFrame == "6 months" || timeFrame == "1 year") {
        return "MMM yyyy";
    } else {
        return "MMM yyyy";
    }
}

void TrendsDialog::setAxisRangeAndTicksForParameter(const QString &parameter, QValueAxis *axis)
{
    if (parameter == "Nitrate") {
        axis->setRange(0, 100);
        axis->setTickCount(11);
    } else if (parameter == "Ammonia") {
        axis->setRange(0, 1);
        axis->setTickCount(11);
    } else {
        axis->setRange(0, 100);
        axis->setTickCount(11);
    }
}

int TrendsDialog::getTankIdByName(const QString &tankName)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM tanks WHERE name = :name");
    query.bindValue(":name", tankName);

    if (!query.exec()) {
        qDebug() << "Failed to fetch tank ID:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return -1;
}
