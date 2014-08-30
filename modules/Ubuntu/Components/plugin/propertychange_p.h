/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef PROPERTYCHANGE_P_H
#define PROPERTYCHANGE_P_H

#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtQml/QQmlProperty>

class QQmlAbstractBinding;
class PropertyChange
{
public:
    explicit PropertyChange();
    PropertyChange(QObject *item, const char *property, const QVariant &value);
    ~PropertyChange();

    void backupAndSet(QObject *item, const char *property, const QVariant &value);
    void restore();
private:
    bool m_backedUp;
    QQmlProperty qmlProperty;
    QPair<QQmlAbstractBinding*, QVariant> backup;
};

#endif // PROPERTYCHANGE_P_H
