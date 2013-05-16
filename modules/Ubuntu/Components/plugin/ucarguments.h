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
#include <QtQml/QQmlParserStatus>

class UCArguments : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(UCArgument* defaultArgument READ defaultArgument WRITE setDefaultArgument NOTIFY defaultArgumentChanged)
    Q_PROPERTY(QQmlListProperty<UCArgument> arguments READ arguments)
    Q_CLASSINFO("DefaultProperty", "arguments")

public:
    explicit UCArguments(QObject *parent = 0);

    UCArgument* defaultArgument() const;
    void setDefaultArgument(UCArgument* argument);
    QQmlListProperty<UCArgument> arguments();
    Q_INVOKABLE void printUsageAndQuit(QString errorMessage = QString());

    // methods necessary for implementing property QQmlListProperty arguments
    void appendArguments(UCArgument* argument);
    UCArgument* atArguments(int index);
    int countArguments();
    void clearArguments();

    // inherited from QQmlParserStatus
    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void defaultArgumentChanged();

protected:
    void parseAndExposeArguments();
    QHash<QString, QStringList> buildExpectedArguments(QList<UCArgument*> declaredArguments);
    QHash<QString, QStringList> parseRawArguments(QStringList rawArguments, QHash<QString, QStringList> expectedArguments);
    void exposeArgumentsAsProperties(QHash<QString, QStringList> argumentsValues);

private:
    bool m_completed;
    UCArgument* m_defaultArgument;
    QList<UCArgument*> m_arguments;
    QStringList m_rawArguments;
    QString m_applicationBinary;
};

#endif // UCARGUMENTS_H
