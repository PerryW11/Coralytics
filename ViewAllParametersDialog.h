#ifndef VIEWALLPARAMETERSDIALOG_H
#define VIEWALLPARAMETERSDIALOG_H

#include <QDialog>
#include <QTableWidget>

class ViewAllParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewAllParametersDialog(const QString &tankName, QWidget *parent = nullptr);

private:
    QTableWidget *parametersTable;
    void loadAllParameters(const QString &tankName);
};

#endif // VIEWALLPARAMETERSDIALOG_H
