#include "TaskItemWidget.h"
#include <QHBoxLayout>

TaskItemWidget::TaskItemWidget(const QString &description, const QString &dueDate, QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    descriptionLabel = new QLabel(QString("%1 (Due: %2)").arg(description).arg(dueDate), this);
    layout->addWidget(descriptionLabel);

    editButton = new QPushButton("Edit", this);
    editButton->setFixedWidth(50);
    layout->addWidget(editButton);

    completeButton = new QPushButton("Complete", this);
    completeButton->setFixedWidth(75);
    layout->addWidget(completeButton);

    QPushButton *deleteButton = new QPushButton("Delete", this);
    deleteButton->setFixedWidth(75);
    layout->addWidget(deleteButton);

    connect(editButton, &QPushButton::clicked, [this, description]() {
        emit editClicked(description);
    });

    connect(completeButton, &QPushButton::clicked, [this, description]() {
        emit completeClicked(description);
    });

    connect(deleteButton, &QPushButton::clicked, [this, description]() {
        emit deleteClicked(description);
    });

    setLayout(layout);
}

