/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UBUNTU_COMPONENTS_LISTENER_H
#define UBUNTU_COMPONENTS_LISTENER_H

#include <QObject>

class QQmlContext;
class QQmlProperty;

class ContextPropertyChangeListener : public QObject
{
    Q_OBJECT
public:
    explicit ContextPropertyChangeListener(QQmlContext* context, const QString& contextProperty);
    ~ContextPropertyChangeListener();
    void setUpdaterProperty(QObject *sender, const char *property);
    Q_SLOT void updateContextProperty();
    Q_SLOT void updateContextPropertyFromSenderProperty();
    QQmlContext* m_context;
    QString m_contextProperty;
    QObject *m_sender;
    QQmlProperty *m_property;
};

#endif // UBUNTU_COMPONENTS_PLUGIN_H
