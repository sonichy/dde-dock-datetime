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

#include "datetimeplugin.h"
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>

DatetimePlugin::DatetimePlugin(QObject *parent)
    : QObject(parent),
      m_dateTipsLabel(new QLabel),
      m_refershTimer(new QTimer(this)),
      m_settings("deepin", "dde-dock-datetime1")
{
    m_dateTipsLabel->setObjectName("datetime1");
    m_dateTipsLabel->setStyleSheet("color:white; padding:0px 3px;");

    m_refershTimer->setInterval(1000);
    m_refershTimer->start();

    m_centralWidget = new DatetimeWidget;

    connect(m_centralWidget, &DatetimeWidget::requestUpdateGeometry, [this] { m_proxyInter->itemUpdate(this, pluginName()); });

    connect(m_refershTimer, &QTimer::timeout, this, &DatetimePlugin::updateCurrentTimeString);

    calendarApplet = new CalendarWidget;

}

const QString DatetimePlugin::pluginName() const
{
    return "datetime1";
}

const QString DatetimePlugin::pluginDisplayName() const
{
    return tr("日期时间");
}

void DatetimePlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;

    if (m_centralWidget->enabled())
        m_proxyInter->itemAdded(this, pluginName());
}

void DatetimePlugin::pluginStateSwitched()
{
    m_centralWidget->setEnabled(!m_centralWidget->enabled());

    if (m_centralWidget->enabled())
        m_proxyInter->itemAdded(this, pluginName());
    else
        m_proxyInter->itemRemoved(this, pluginName());
}

bool DatetimePlugin::pluginIsDisable()
{
    return !m_centralWidget->enabled();
}

int DatetimePlugin::itemSortKey(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    const QString key = QString("pos_%1").arg(displayMode());
    return m_settings.value(key, 0).toInt();
}

void DatetimePlugin::setSortKey(const QString &itemKey, const int order)
{
    Q_UNUSED(itemKey);

    const QString key = QString("pos_%1").arg(displayMode());
    m_settings.setValue(key, order);
}

QWidget *DatetimePlugin::itemWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    return m_centralWidget;
}

QWidget *DatetimePlugin::itemTipsWidget(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    return m_dateTipsLabel;
}

QWidget *DatetimePlugin::itemPopupApplet(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return calendarApplet;
}

const QString DatetimePlugin::itemContextMenu(const QString &itemKey)
{
    Q_UNUSED(itemKey);

    QList<QVariant> items;
    items.reserve(4);

    QMap<QString, QVariant> set;
    set["itemId"] = "set";
    set["itemText"] = tr("Set");
    set["isActive"] = true;
    items.push_back(set);

    QMap<QString, QVariant> settings;
    settings["itemId"] = "settings";
    if (m_centralWidget->is24HourFormat())
        settings["itemText"] = tr("12 Hour Time");
    else
        settings["itemText"] = tr("24 Hour Time");
    settings["isActive"] = true;
    items.push_back(settings);

    QMap<QString, QVariant> open;
    open["itemId"] = "open";
    open["itemText"] = tr("Time Settings");
    open["isActive"] = true;
    items.push_back(open);

    QMap<QString, QVariant> clock;
    clock["itemId"] = "clock";
    clock["itemText"] = tr("Clock");
    clock["isActive"] = true;
    items.push_back(clock);

    QMap<QString, QVariant> menu;
    menu["items"] = items;
    menu["checkableMenu"] = false;
    menu["singleCheck"] = false;

    return QJsonDocument::fromVariant(menu).toJson();
}

void DatetimePlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(itemKey)
    Q_UNUSED(checked)

    if (menuId == "open")
        QProcess::startDetached("dbus-send --print-reply --dest=com.deepin.dde.ControlCenter /com/deepin/dde/ControlCenter com.deepin.dde.ControlCenter.ShowModule \"string:datetime\"");
    if (menuId == "clock")
        m_centralWidget->toggleClock();
    if (menuId == "set")
        set();
    if (menuId == "settings")
        m_centralWidget->toggleHourFormat();
}

void DatetimePlugin::updateCurrentTimeString()
{
    const QDateTime currentDateTime = QDateTime::currentDateTime();

    if (m_centralWidget->is24HourFormat())
        m_dateTipsLabel->setText(currentDateTime.date().toString(Qt::SystemLocaleLongDate) + currentDateTime.toString(" HH:mm:ss"));
    else
        m_dateTipsLabel->setText(currentDateTime.date().toString(Qt::SystemLocaleLongDate) + currentDateTime.toString(" hh:mm:ss A"));

    const QString currentString = currentDateTime.toString("mm");

    //if (currentString == m_currentTimeString)
    //    return;

    m_currentTimeString = currentString;
    m_centralWidget->update();
}

void DatetimePlugin::set()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Set");
    dialog->setFixedWidth(300);
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    QLabel *label = new QLabel("Format");
    hbox->addWidget(label);
    QLineEdit *lineEdit_format = new QLineEdit;
    lineEdit_format->setText(m_settings.value("format","yyyy/M/d\\nHH:mm ddd").toString());
    hbox->addWidget(lineEdit_format);
    vbox->addLayout(hbox);

    hbox = new QHBoxLayout;
    label = new QLabel("Such as:");
    hbox->addWidget(label);
    label = new QLabel("yyyy/M/d\\nHH:mm ddd\nHH:mm ddd\\nyyyy/M/d");
    hbox->addWidget(label);
    vbox->addLayout(hbox);

    hbox = new QHBoxLayout;
    QPushButton *pushButton_confirm = new QPushButton("Confirm");
    QPushButton *pushButton_cancel = new QPushButton("Cancel");
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(pushButton_cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
    hbox->addWidget(pushButton_confirm);
    hbox->addWidget(pushButton_cancel);
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    if(dialog->exec() == QDialog::Accepted){
        m_settings.setValue("format", lineEdit_format->text());
        m_centralWidget->update();
    }
    dialog->close();
}