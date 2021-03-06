/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dde-dock/constants.h"
#include "datetimewidget.h"
#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QSvgRenderer>
#include <QMouseEvent>

#define PLUGIN_STATE_KEY    "enable"

DatetimeWidget::DatetimeWidget(QWidget *parent)
    : QWidget(parent),
      m_settings("deepin", "dde-dock-datetime1"),
      m_24HourFormat(m_settings.value("24HourFormat", true).toBool())
{
}

bool DatetimeWidget::enabled()
{
    return m_settings.value(PLUGIN_STATE_KEY, true).toBool();
}

void DatetimeWidget::setEnabled(const bool b)
{
    m_settings.setValue(PLUGIN_STATE_KEY, b);
}

void DatetimeWidget::toggleHourFormat()
{
    m_24HourFormat = !m_24HourFormat;
    m_settings.setValue("24HourFormat", m_24HourFormat);
    m_cachedTime.clear();
    update();
    emit requestUpdateGeometry();
    m_settings.setValue("ShowClock", false);
}

void DatetimeWidget::toggleClock()
{
    m_settings.setValue("ShowClock", true);
    update();
}

QSize DatetimeWidget::sizeHint() const
{
    const Dock::DisplayMode displayMode = qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    if (displayMode == Dock::Efficient) {
        QFontMetrics FM(qApp->font());
        if (m_24HourFormat){
            QString format = m_settings.value("format", "yyyy/M/d\nHH:mm ddd").toString();
            if(format.contains("\\n")){
                QStringList SL = format.split("\\n");
                if(SL.at(0).length() > SL.at(1).length())
                    return FM.boundingRect(SL.at(0)).size() + QSize(0, FM.boundingRect(SL.at(1)).height());
                else
                    return FM.boundingRect(SL.at(1)).size() + QSize(0, FM.boundingRect(SL.at(1)).height());
            }else
                //return FM.boundingRect("yyyy-MM-dd").size() + QSize(0, FM.boundingRect("HH:mm ddd").height());
                return FM.boundingRect(format).size() + QSize(0, FM.boundingRect(format).height());
        }
        else
            return FM.boundingRect("88:88 A.A.").size() + QSize(20, 20);
    }else{
        const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
        if (position == Dock::Top || position == Dock::Bottom)
            return QSize(height(), height());
        else
            return QSize(width(), width());
    }
}

void DatetimeWidget::resizeEvent(QResizeEvent *e)
{
    m_cachedTime.clear();
    QWidget::resizeEvent(e);
}

void DatetimeWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    const auto ratio = qApp->devicePixelRatio();
    const Dock::DisplayMode displayMode = qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    const QDateTime current = QDateTime::currentDateTime();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (displayMode == Dock::Efficient)
    {
        QString format;
        if (m_24HourFormat)
            // format = "hh:mm";
            format = m_settings.value("format","yyyy/M/d\nHH:mm ddd").toString().replace("\\n","\n");
        else
        {
            if (position == Dock::Top || position == Dock::Bottom)
                format = "hh:mm AP";
            else
                format = "hh:mm\nAP";
        }

        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, current.toString(format).replace("周",""));
        return;
    }

    // clock
    if(m_settings.value("ShowClock",false).toBool()){
        int w = qMin(width(), height());
        int h = w;
        QPixmap pixmap(w,h);
        pixmap.fill(Qt::transparent);
        QPainter painter1(&pixmap);
        painter1.setRenderHint(QPainter::Antialiasing);
        // face
        painter1.setPen(QPen(Qt::black, w*0.02));
        painter1.setBrush(QBrush(Qt::white));
        painter1.drawEllipse(QPoint(w/2,h/2), (int)(w/2*0.9), (int)(h/2*0.9));
        painter1.setBrush(QBrush(Qt::black));
        painter1.drawEllipse(QPoint(w/2,h/2), (int)(w*0.015), (int)(h*0.015));
        qreal da = 2 * M_PI / 60;
        QTime time = QTime::currentTime();
        int hour = time.hour();
        int m = time.minute();
        int s = time.second();
        // second hand
        int x = w * 0.4 * qCos(M_PI/2 - s * da) + w / 2;
        int y = - h * 0.4 * qSin(M_PI/2 - s * da) + h / 2;
        painter1.setPen(QPen(Qt::black, w*0.01));
        painter1.drawLine(QPoint(w/2,h/2), QPoint(x,y));
        // minute hand
        x = w * 0.35 * qCos(M_PI/2 - m * da - s * da / 60) + w / 2;
        y = - h * 0.35 * qSin(M_PI/2 - m * da - s * da / 60) + h / 2;
        painter1.setPen(QPen(Qt::black, w*0.02));
        painter1.drawLine(QPoint(w/2,h/2), QPoint(x,y));
        // hour hand
        da = 2 * M_PI / 12;
        if(hour >= 12) hour -= 12;
        x = w*0.25 * qCos(M_PI/2 - hour * da - m * da / 60) + w/2;
        y = - h*0.25 * qSin(M_PI/2 - hour * da - m * da / 60) + h/2;
        painter1.setPen(QPen(Qt::black, w*0.03));
        painter1.drawLine(QPoint(w/2,h/2), QPoint(x,y));
        painter.drawPixmap(rect().center() - pixmap.rect().center(), pixmap);
        return;
    }

    const QString currentTimeString = current.toString(m_24HourFormat ? "hhmm" : "hhmma");
    // check cache valid
    if (m_cachedTime != currentTimeString)
    {
        m_cachedTime = currentTimeString;

        // draw new pixmap
        m_cachedTime = currentTimeString;
        m_cachedIcon = QPixmap(size() * ratio);
        m_cachedIcon.fill(Qt::transparent);
        m_cachedIcon.setDevicePixelRatio(ratio);
        QPainter p(&m_cachedIcon);

        // draw fashion mode datetime plugin
        const int perfectIconSize = qMin(width(), height()) * 0.8;
        const QRect r = rect();

        // draw background
        QPixmap background = loadSvg(":/icons/resources/icons/background.svg", QSize(perfectIconSize, perfectIconSize));
        const QPoint backgroundOffset = r.center() - background.rect().center() / ratio;
        p.drawPixmap(backgroundOffset, background);

        const int bigNumHeight = perfectIconSize / 2.5;
        const int bigNumWidth = double(bigNumHeight) * 8 / 18;
        const int smallNumHeight = bigNumHeight / 2;
        const int smallNumWidth = double(smallNumHeight) * 5 / 9;

        // draw big num 1
        const QString bigNum1Path = QString(":/icons/resources/icons/big%1.svg").arg(currentTimeString[0]);
        const QPixmap bigNum1 = loadSvg(bigNum1Path, QSize(bigNumWidth, bigNumHeight));
        const QPoint bigNum1Offset = backgroundOffset + QPoint(perfectIconSize / 2 - bigNumWidth * 2 + 1, perfectIconSize / 2 - bigNumHeight / 2);
        p.drawPixmap(bigNum1Offset, bigNum1);

        // draw big num 2
        const QString bigNum2Path = QString(":/icons/resources/icons/big%1.svg").arg(currentTimeString[1]);
        const QPixmap bigNum2 = loadSvg(bigNum2Path, QSize(bigNumWidth, bigNumHeight));
        const QPoint bigNum2Offset = bigNum1Offset + QPoint(bigNumWidth + 1, 0);
        p.drawPixmap(bigNum2Offset, bigNum2);

        if (!m_24HourFormat)
        {
            // draw small num 1
            const QString smallNum1Path = QString(":/icons/resources/icons/small%1.svg").arg(currentTimeString[2]);
            const QPixmap smallNum1 = loadSvg(smallNum1Path, QSize(smallNumWidth, smallNumHeight));
            const QPoint smallNum1Offset = bigNum2Offset + QPoint(bigNumWidth + 2, 1);
            p.drawPixmap(smallNum1Offset, smallNum1);

            // draw small num 2
            const QString smallNum2Path = QString(":/icons/resources/icons/small%1.svg").arg(currentTimeString[3]);
            const QPixmap smallNum2 = loadSvg(smallNum2Path, QSize(smallNumWidth, smallNumHeight));
            const QPoint smallNum2Offset = smallNum1Offset + QPoint(smallNumWidth + 1, 0);
            p.drawPixmap(smallNum2Offset, smallNum2);

            // draw am/pm tips
            const int tips_width = (smallNumWidth * 2 + 2) & ~0x1;
            const int tips_height = tips_width / 2;

            QPixmap tips;
            if (current.time().hour() > 12)
                tips = loadSvg(":/icons/resources/icons/tips-pm.svg", QSize(tips_width, tips_height));
            else
                tips = loadSvg(":/icons/resources/icons/tips-am.svg", QSize(tips_width, tips_height));

            const QPoint tipsOffset = bigNum2Offset + QPoint(bigNumWidth + 2, bigNumHeight - tips_height);
            p.drawPixmap(tipsOffset, tips);
        } else {
            // draw small num 1
            const QString smallNum1Path = QString(":/icons/resources/icons/small%1.svg").arg(currentTimeString[2]);
            const QPixmap smallNum1 = loadSvg(smallNum1Path, QSize(smallNumWidth, smallNumHeight));
            const QPoint smallNum1Offset = bigNum2Offset + QPoint(bigNumWidth + 2, smallNumHeight);
            p.drawPixmap(smallNum1Offset, smallNum1);

            // draw small num 2
            const QString smallNum2Path = QString(":/icons/resources/icons/small%1.svg").arg(currentTimeString[3]);
            const QPixmap smallNum2 = loadSvg(smallNum2Path, QSize(smallNumWidth, smallNumHeight));
            const QPoint smallNum2Offset = smallNum1Offset + QPoint(smallNumWidth + 1, 0);
            p.drawPixmap(smallNum2Offset, smallNum2);
        }
    }

    // draw cached fashion mode time item
    painter.drawPixmap(rect().center() - m_cachedIcon.rect().center() / ratio, m_cachedIcon);
}

const QPixmap DatetimeWidget::loadSvg(const QString &fileName, const QSize size)
{
    const auto ratio = qApp->devicePixelRatio();

    QPixmap pixmap(size * ratio);
    QSvgRenderer renderer(fileName);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(ratio);

    return pixmap;
}
