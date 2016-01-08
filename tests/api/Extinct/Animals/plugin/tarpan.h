/*
 * Copyright 2015 Canonical Ltd.
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
 *
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#ifndef UCTARPAN_H
#define UCTARPAN_H

#include <QtCore/QObject>
#include <QtCore/QString>

class EATarpan : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit EATarpan(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    Q_INVOKABLE void kick() const;

Q_SIGNALS:
    void nameChanged();

private:
    QString m_name;
};

#endif // UCTARPAN_H
