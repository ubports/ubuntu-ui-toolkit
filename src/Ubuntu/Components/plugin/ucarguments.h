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
#include <QtQml/QQmlPropertyMap>
#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>

class UCArguments : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(UCArgument* defaultArgument READ defaultArgument WRITE setDefaultArgument NOTIFY defaultArgumentChanged)
    Q_PROPERTY(QQmlListProperty<UCArgument> arguments READ arguments)
    Q_CLASSINFO("DefaultProperty", "arguments")
    Q_PROPERTY(QQmlPropertyMap* values READ values CONSTANT)
    Q_PROPERTY(bool error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit UCArguments(QObject *parent = 0);

    UCArgument* defaultArgument() const;
    void setDefaultArgument(UCArgument* argument);
    QQmlPropertyMap* values() const;
    bool error() const;
    QString errorMessage() const;
    Q_INVOKABLE void printUsage();
    Q_INVOKABLE void quitWithError(const QString& errorMessage = QString());

    // inherited from QQmlParserStatus
    void classBegin();
    void componentComplete();

    // methods necessary for implementing property QQmlListProperty arguments
    void appendArguments(UCArgument* argument);
    UCArgument* atArguments(int index);
    int countArguments();
    void clearArguments();
    QQmlListProperty<UCArgument> arguments();

Q_SIGNALS:
    void defaultArgumentChanged();
    void errorChanged();
    void errorMessageChanged();

protected:
    void parseAndExposeArguments();
    void setValuesOnArguments(const QHash<QString, QStringList>& argumentsValues);
    QHash<QString, QStringList> buildExpectedArguments(const QList<UCArgument*>& declaredArguments);
    QHash<QString, QStringList> parseRawArguments(const QStringList& rawArguments, const QHash<QString, QStringList>& expectedArguments);
    QStringList collectArgumentValues(QStringList::const_iterator& i, QStringList::const_iterator end, int nValues);
    bool usageRequested(const QStringList& argumentNames);
    bool requiredArgumentsProvided(const QHash<QString, QStringList>& argumentsValues, QString& error);
    bool requiredDefaultArgumentProvided(const QHash<QString, QStringList>& argumentsValues, QString& error);
    void exposeArgumentsAsProperties(const QHash<QString, QStringList>& argumentsValues);
    QString usage();
    void setErrorMessage(const QString& errorMessage);
    void quitApplication();

protected Q_SLOTS:
    void warningArgumentPropertyChanged();

private:
    bool m_completed;
    UCArgument* m_defaultArgument;
    QList<UCArgument*> m_arguments;
    QStringList m_rawArguments;
    QString m_applicationBinary;
    QQmlPropertyMap* m_values;
    bool m_error;
    QString m_errorMessage;
};

#endif // UCARGUMENTS_H
