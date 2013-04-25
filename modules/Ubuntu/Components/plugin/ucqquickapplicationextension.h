/*
 * Copyright 2013 Canonical Ltd.
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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#ifndef QQUICKAPPLICATIONEXTENSION_H
#define QQUICKAPPLICATIONEXTENSION_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class UCQQuickApplicationExtension : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList arguments READ arguments NOTIFY argumentsChanged)

public:
    static UCQQuickApplicationExtension& instance()
    {
        static UCQQuickApplicationExtension instance;
        return instance;
    }

    explicit UCQQuickApplicationExtension(QObject *parent = 0);

    QStringList arguments() const;

Q_SIGNALS:
    void argumentsChanged(const QStringList &);
};

#endif // QQUICKAPPLICATIONEXTENSION_H
