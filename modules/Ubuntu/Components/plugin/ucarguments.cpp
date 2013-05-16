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

#include "ucarguments.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtQml/QQmlProperty>
#include <QtQml/qqml.h>

UCArguments::UCArguments(QObject *parent) :
    QObject(parent),
    m_completed(false),
    m_defaultArgument(NULL)
{
    m_rawArguments = QCoreApplication::arguments();
    //qDebug() << "RAW ARGUMENTS" << m_rawArguments; // FIXME: remove
}

UCArgument* UCArguments::defaultArgument() const
{
    return m_defaultArgument;
}

void UCArguments::setDefaultArgument(UCArgument* argument)
{
    // FIXME: finish implementation
    m_defaultArgument = argument;
    Q_EMIT(defaultArgumentChanged());
}


void UCArguments::appendArguments(UCArgument* argument)
{
    m_arguments.append(argument);
    if (m_completed) {
        parseAndExposeArguments();
    }
}

UCArgument* UCArguments::atArguments(int index)
{
    return m_arguments.at(index);
}

int UCArguments::countArguments()
{
    return m_arguments.size();
}

void UCArguments::clearArguments()
{
    m_arguments.clear();
    parseAndExposeArguments();
}

void staticAppendArguments(QQmlListProperty<UCArgument>* property, UCArgument* argument)
{
    UCArguments* arguments = static_cast<UCArguments*>(property->data);
    arguments->appendArguments(argument);
}

UCArgument* staticAtArguments(QQmlListProperty<UCArgument>* property, int index)
{
    UCArguments* arguments = static_cast<UCArguments*>(property->data);
    return arguments->atArguments(index);
}

int staticCountArguments(QQmlListProperty<UCArgument>* property)
{
    UCArguments* arguments = static_cast<UCArguments*>(property->data);
    return arguments->countArguments();
}

void staticClearArguments(QQmlListProperty<UCArgument>* property)
{
    UCArguments* arguments = static_cast<UCArguments*>(property->data);
    arguments->clearArguments();
}

QQmlListProperty<UCArgument> UCArguments::arguments()
{
    return QQmlListProperty<UCArgument>(this, this, &staticAppendArguments,
                                        &staticCountArguments, &staticAtArguments,
                                        &staticClearArguments);
}

void UCArguments::printUsageAndQuit(QString errorMessage)
{
    /* This function can be called multiple times before the application actually
       quit. See comment at the end about QCoreApplication::quit().
    */
    static bool alreadyCalled = false;
    if (alreadyCalled) {
        return;
    }
    alreadyCalled = true;

    // FIXME: use i18n
    QLatin1String indentation("  ");
    QString usage;
    QTextStream usageStream(&usage);
    // FIXME: guess application binary better
    QString applicationBinary = m_rawArguments[0];

    usageStream << QString("Usage: ");
    usageStream << applicationBinary;

    Q_FOREACH (UCArgument* argument, m_arguments) {
        usageStream << " ";
        if (argument->required()) {
            usageStream << QString("[");
        }

        // FIXME: support more than one value per argument
        usageStream << QString("--") << argument->name();
        if (!argument->valueNames().empty()) {
            usageStream << "=" << argument->valueNames()[0];
        }

        if (argument->required()) {
            usageStream << QString("[");
        }
    }

    usageStream << endl;
    usageStream << QString("Options:") << endl;

    Q_FOREACH (UCArgument* argument, m_arguments) {
        // ensure that the help string for the arguments are aligned vertically
        QString spacing = QString(" ").repeated(20 - argument->name().size());
        usageStream << indentation << QString("--") << argument->name() << spacing << argument->help() << endl;
    }

    if (m_defaultArgument != NULL) {
        usageStream << endl << indentation << m_defaultArgument->help();
    }

    if (!errorMessage.isEmpty()) {
        // convert to char* to avoid qWarning printing out quotes ""
        qWarning() << errorMessage.toStdString().c_str();
    }
    // convert to char* to avoid qWarning printing out quotes ""
    qWarning() << usage.toStdString().c_str();

    /* Call QCoreApplication::quit() when the event loop starts running.
       "If the event loop is not running, [QCoreApplication::quit()] does nothing."
       Ref.: http://qt-project.org/doc/qt-5.0/qtcore/qcoreapplication.html#exit
    */
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
}

void UCArguments::classBegin()
{
}

void UCArguments::componentComplete()
{
    m_completed = true;
    parseAndExposeArguments();
}

void UCArguments::parseAndExposeArguments()
{
    QHash<QString, QStringList> expectedArguments;
    QHash<QString, QStringList> argumentsValues;

    expectedArguments = buildExpectedArguments(m_arguments);
    argumentsValues = parseRawArguments(m_rawArguments, expectedArguments);

    if (argumentsValues.contains("help") ||
        argumentsValues.contains("h") ||
        argumentsValues.contains("usage")) {
        printUsageAndQuit();
    }

    exposeArgumentsAsProperties(argumentsValues);
}

QHash<QString, QStringList> UCArguments::buildExpectedArguments(QList<UCArgument*> declaredArguments)
{
    QHash<QString, QStringList> expectedArguments;
    UCArgument* argument;

    Q_FOREACH (argument, declaredArguments) {
        expectedArguments.insert(argument->name(), argument->valueNames());
    }

    return expectedArguments;
}

QHash<QString, QStringList> UCArguments::parseRawArguments(QStringList rawArguments, QHash<QString, QStringList> expectedArguments)
{
    // FIXME: break down in smaller functions
    QHash<QString, QStringList> argumentsValues;
    QString name;
    QStringList values;

    // ignore the first item in the rawArguments which is the name of the binary
    QStringList::const_iterator i;
    for (i = rawArguments.constBegin(), ++i; i != rawArguments.constEnd(); ++i) {
        QString rawArgument = (*i);
        if (rawArgument.startsWith('-')) {
            if (!name.isEmpty()) {
                // insert values of previously parsed named argument
                argumentsValues.insert(name, values);
            }
            // it starts with a '-' therefore it is a named argument
            // remove all prepended '-'
            rawArgument = rawArgument.split('-', QString::SkipEmptyParts).join('-');
            // string after the '=' sign is a value for the argument
            values = rawArgument.split("=");
            name = values.takeAt(0);
        } else {
            if (!expectedArguments.contains(name) && values.empty()) {
                // unexpected named arguments are given at most one value
                values.append(rawArgument);
            } else if (values.size() < expectedArguments[name].size()) {
                // additional value for a named argument
                values.append(rawArgument);
            } else {
                if (!name.isEmpty()) {
                    // insert values of previously parsed named argument
                    argumentsValues.insert(name, values);
                }
                // default/unnamed argument
                argumentsValues[""].append(rawArgument);
            }
        }
    }

    // FIXME: code duplicated 3 times
    if (!name.isEmpty()) {
        // insert values of previously parsed named argument
        argumentsValues.insert(name, values);
    }

    //qDebug() << "PARSING... "  << argumentsValues; // FIXME: remove
    return argumentsValues;
}

void UCArguments::exposeArgumentsAsProperties(QHash<QString, QStringList> argumentsValues)
{
    QHash<QString, QStringList>::const_iterator i;

    for (i = argumentsValues.constBegin(); i != argumentsValues.constEnd(); ++i) {
        QString name = i.key();
        if (name.isEmpty()) {
            continue;
        }

        QStringList values = i.value();
        QQmlProperty qmlProperty(this, name, qmlContext(this));
        const char* propertyName = name.toStdString().c_str();
        QVariant value;

        if (values.size() == 0) {
            value.setValue(true);
        } else if (values.size() == 1) {
            value.setValue(values.at(0));
        } else {
            value.setValue(values);
        }

        //qDebug() << "EXPOSING"  << name << value; // FIXME: remove
        // necessary for the value to be set to the QML property
        // FIXME: could spit out warnings if the QML property was not defined,
        // ie. if the write() returns false
        qmlProperty.write(value);
        // necessary for the value to be set to the C++ dynamic property
        setProperty(propertyName, value); // FIXME: is it necessary?
    }
}
