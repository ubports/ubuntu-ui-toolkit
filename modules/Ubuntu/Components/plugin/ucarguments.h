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
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QPair>
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

    // methods necessary for implementing property QQmlListProperty arguments
    void appendArguments(UCArgument* argument);
    UCArgument* atArguments(int index);
    int countArguments();
    void clearArguments();

Q_SIGNALS:
    void defaultArgumentChanged();

protected:
    QHash<QString, QStringList> buildExpectedArguments(QList<UCArgument*> declaredArguments);
    QHash<QString, QStringList> parseRawArguments(QStringList rawArguments);

private:
    UCArgument* m_defaultArgument;
    QList<UCArgument*> m_arguments;
    QStringList m_rawArguments;
    QHash<QString, QStringList> m_argumentsValues;
    QHash<QString, QStringList> m_expectedArguments;
};

#endif // UCARGUMENTS_H
