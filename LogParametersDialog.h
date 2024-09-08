#ifndef LOGPARAMETERSDIALOG_H
#define LOGPARAMETERSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QMap>

class LogParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogParametersDialog(QWidget *parent = nullptr);
    QMap<QString, double> getParameters() const;

private slots:
    void addParameter();

private:
    QVBoxLayout *parametersLayout;
    QMap<QString, QLineEdit*> parametersMap;
    QComboBox *parametersComboBox;
    QPushButton *addButton;
    QPushButton *logButton;
    QPushButton *cancelButton;
};

#endif // LOGPARAMETERSDIALOG_H
