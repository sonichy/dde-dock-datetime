#include "calendarwidget.h"

#include <QVBoxLayout>

CalendarWidget::CalendarWidget(QWidget *parent) : QWidget(parent)
{
    calendar = new QCalendarWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(calendar);
    setLayout(vbox);
}
