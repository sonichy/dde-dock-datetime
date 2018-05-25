#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QCalendarWidget>

class CalendarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CalendarWidget(QWidget *parent = nullptr);
    QCalendarWidget *calendar;

signals:

public slots:
};

#endif // CALENDARWIDGET_H
