#ifndef TRENDSDIALOG_H
#define TRENDSDIALOG_H

#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>

    class TrendsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrendsDialog(const QString &tankName, QWidget *parent = nullptr);
    ~TrendsDialog();

private slots:
    void onParameterChanged();
    void onTimeFrameChanged();
    void generateGraph();

private:
    QString tankName;
    QVBoxLayout *layout;
    QComboBox *parameterComboBox;
    QComboBox *timeFrameComboBox;

    int getTickCountForTimeFrame(const QString &timeFrame);
    QString getDateFormatForTimeFrame(const QString &timeFrame);
    void setAxisRangeAndTicksForParameter(const QString &parameter, QValueAxis *axis);
    int getTankIdByName(const QString &tankName);
    void populateParameters();
    bool parameterHasData(const QString &parameter);
};

#endif // TRENDSDIALOG_H
