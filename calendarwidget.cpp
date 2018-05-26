#include "calendarwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

CalendarWidget::CalendarWidget(QWidget *parent) : QWidget(parent)
{    
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addStretch();
    QPushButton *pushButton_today = new QPushButton("今天");
    connect(pushButton_today,SIGNAL(pressed()),this,SLOT(jumpToToday()));
    hbox->addWidget(pushButton_today);
    vbox->addLayout(hbox);
    calendar = new QCalendarWidget;
    vbox->addWidget(calendar);
    setLayout(vbox);
}

void CalendarWidget::jumpToToday()
{
    calendar->setSelectedDate(QDate::currentDate());
}
