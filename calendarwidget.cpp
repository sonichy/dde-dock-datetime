#include "calendarwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QPainter>
#include <QtMath>
#include <QApplication>

CalendarWidget::CalendarWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addStretch();
    QPushButton *pushButton_today = new QPushButton("今");
    pushButton_today->setFixedWidth(30);
    connect(pushButton_today,SIGNAL(pressed()),this,SLOT(jumpToToday()));
    hbox->addWidget(pushButton_today);
    vbox->addLayout(hbox);
    hbox = new QHBoxLayout;
    calendar = new QCalendarWidget;
    calendar->setStyleSheet("QTableView { alternate-background-color:#101010; }");
    hbox->addWidget(calendar);
    label = new QLabel;
    label->setFixedSize(200,200);
    hbox->addWidget(label);
    vbox->addLayout(hbox);
    setLayout(vbox);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(drawClock()));
    timer->start(1000);
}

void CalendarWidget::jumpToToday()
{
    calendar->setSelectedDate(QDate::currentDate());
}

void CalendarWidget::drawClock()
{
    int w = label->width();
    int h = label->height();
    QPixmap pixmap(w, h);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing,true);
    // face
    painter.setPen(QPen(Qt::black, w*0.02));
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(QPoint(w/2,h/2), (int)(w/2*0.9), (int)(h/2*0.9));
    painter.setBrush(QBrush(Qt::black));
    painter.drawEllipse(QPoint(w/2,h/2), (int)(w*0.015), (int)(h*0.015));
    // mark
    qreal da = 2 * M_PI / 60;
    for(int i=0; i<60; i++){
        int r = w * 0.415;
        if(i % 5 == 0) r = w * 0.375;
        int x1 = r * qCos(M_PI/2 - i * da) + w/2;
        int y1 = - r * qSin(M_PI/2 - i * da) + h/2;
        int x2 = w * 0.425 * qCos(M_PI/2 - i * da) + w/2;
        int y2 = - h * 0.425 * qSin(M_PI/2 - i * da) + h/2;
        painter.setPen(QPen(Qt::black,2));
        painter.drawLine(QPoint(x1,y1), QPoint(x2,y2));
    }

    QTime time = QTime::currentTime();
    int hour = time.hour();
    int m = time.minute();
    int s = time.second();
    QFontMetrics FM(qApp->font());
    QString stime = time.toString("HH:mm:ss");
    int fw = FM.width(stime);
    painter.drawText(QPoint((w-fw)/2, w * 0.75), stime);
    // second hand
    int x = w * 0.4 * qCos(M_PI/2 - s * da) + w / 2;
    int y = - h * 0.4 * qSin(M_PI/2 - s * da) + h / 2;
    painter.setPen(QPen(Qt::black, w*0.01));
    painter.drawLine(QPoint(w/2,h/2), QPoint(x,y));
    // minute hand
    x = w * 0.35 * qCos(M_PI/2 - m * da - s * da / 60) + w / 2;
    y = - h * 0.35 * qSin(M_PI/2 - m * da - s * da / 60) + h / 2;
    painter.setPen(QPen(Qt::black, w*0.02));
    painter.drawLine(QPoint(w/2,h/2), QPoint(x,y));
    // hour hand
    da = 2 * M_PI / 12;
    if(hour >= 12) hour -= 12;
    x = w*0.25 * qCos(M_PI/2 - hour * da - m * da / 60) + w/2;
    y = - h*0.25 * qSin(M_PI/2 - hour * da - m * da / 60) + h/2;
    //qDebug() << "x =" << x << ", y =" << y;
    painter.setPen(QPen(Qt::black, w*0.03));
    painter.drawLine(QPoint(w/2,h/2), QPoint(x,y));
    label->setPixmap(pixmap);
}