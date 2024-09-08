#ifndef TASKITEMWIDGET_H
#define TASKITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class TaskItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskItemWidget(const QString &description, const QString &dueDate, QWidget *parent = nullptr);

    QString getDescription() const;

signals:
    void editClicked(const QString &description);
    void completeClicked(const QString &description);
    void deleteClicked(const QString &description);

private:
    QLabel *descriptionLabel;
    QPushButton *editButton;
    QPushButton *completeButton;
};

#endif // TASKITEMWIDGET_H
