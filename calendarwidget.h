#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QCalendarWidget>
#include <QLabel>

class CalendarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CalendarWidget(QWidget *parent = nullptr);
    QCalendarWidget *calendar;
    QLabel *label;

signals:

public slots:
    void jumpToToday();
    void drawClock();

};

#endif // CALENDARWIDGET_H