#ifndef CREATECREATUREDIALOG_H
#define CREATECREATUREDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QPushButton>

class CreateCreatureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateCreatureDialog(QWidget *parent = nullptr);

    QString getSpecies() const;
    QString getName() const;
    double getSize() const;
    QDate getDateAcquired() const;
    QString getNotes() const;

private slots:
    void onAccept();

private:
    QLineEdit *speciesEdit;
    QLineEdit *nameEdit;
    QDoubleSpinBox *sizeSpinBox;
    QDateEdit *dateAcquiredEdit;
    QTextEdit *notesEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // CREATECREATUREDIALOG_H
