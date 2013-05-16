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

#ifndef UCARGUMENTS_H
#define UCARGUMENTS_H

#include "ucargument.h"
#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>

class UCArguments : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UCArgument* defaultArgument READ defaultArgument WRITE setDefaultArgument NOTIFY defaultArgumentChanged)
    Q_PROPERTY(QQmlListProperty<UCArgument> arguments READ arguments)
    Q_CLASSINFO("DefaultProperty", "arguments")

public:
    explicit UCArguments(QObject *parent = 0);

    UCArgument* defaultArgument() const;
    void setDefaultArgument(UCArgument* argument);
    QQmlListProperty<UCArgument> arguments();
    Q_INVOKABLE void quitAndPrintUsage(QString errorMessage);

Q_SIGNALS:
    void defaultArgumentChanged();

private:
    UCArgument* m_defaultArgument;
    QList<UCArgument*> m_arguments;
    QStringList m_rawArguments;
};

#endif // UCARGUMENTS_H
