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
#include "i18n.h"
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
    m_applicationBinary = m_rawArguments[0].split('/').last();
}

UCArgument* UCArguments::defaultArgument() const
{
    return m_defaultArgument;
}

void UCArguments::setDefaultArgument(UCArgument* argument)
{
    m_defaultArgument = argument;
    Q_EMIT(defaultArgumentChanged());
    if (m_completed) {
        parseAndExposeArguments();
    }
}


// FIXME: could be broken down in smaller functions
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

    QLatin1String indentation("  ");
    QString usage;
    QTextStream usageStream(&usage);
    UbuntuI18n* i18n = &UbuntuI18n::instance();

    usageStream << i18n->tr("Usage: ");

    // display overall syntax, for example: program --argument=value DEFAULT_ARGUMENT
    usageStream << m_applicationBinary;

    Q_FOREACH (UCArgument* argument, m_arguments) {
        usageStream << ' ';
        QString syntax = argument->syntax();
        if (!argument->required()) {
            syntax.prepend('[').append(']');
        }
        usageStream << syntax;
    }

    if (m_defaultArgument != NULL) {
        usageStream << ' ';
        usageStream << m_defaultArgument->syntax();
    }

    // display what each argument is used for
    usageStream << endl;
    usageStream << i18n->tr("Options:") << endl;

    Q_FOREACH (UCArgument* argument, m_arguments) {
        usageStream << indentation << argument->usage() << endl;
    }

    if (m_defaultArgument != NULL) {
        usageStream << endl << m_defaultArgument->help();
    }

    // output to the console, first the errorMessage if any then the usage
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
    // FIXME: connect to changed signals for properties: name, required and valueNames
    // for all arguments including defaultArgument and call parseAndExposeArguments
    // when emitted
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

// static callbacks calling the methods above
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
// end of static callbacks


QQmlListProperty<UCArgument> UCArguments::arguments()
{
    return QQmlListProperty<UCArgument>(this, this, &staticAppendArguments,
                                        &staticCountArguments, &staticAtArguments,
                                        &staticClearArguments);
}

void UCArguments::parseAndExposeArguments()
{
    QHash<QString, QStringList> expectedArguments;
    QHash<QString, QStringList> argumentsValues;

    expectedArguments = buildExpectedArguments(m_arguments);
    argumentsValues = parseRawArguments(m_rawArguments, expectedArguments);

    if (usageRequested(argumentsValues.keys())) {
        printUsageAndQuit();
    }

    QString error;
    if (!requiredArgumentsProvided(argumentsValues, error)) {
        printUsageAndQuit(error);
    }

    if (!requiredDefaultArgumentProvided(argumentsValues, error)) {
        printUsageAndQuit(error);
    }

    // pass the values to the arguments objects
    Q_FOREACH (UCArgument* argument, m_arguments) {
        argument->setValues(argumentsValues[argument->name()]);
    }
    if (m_defaultArgument != NULL) {
        m_defaultArgument->setValues(argumentsValues[""]);
        // FIXME: not very elegant way to inform that values have changed
        Q_EMIT(defaultArgumentChanged());
    }

    exposeArgumentsAsProperties(argumentsValues);
}

QHash<QString, QStringList> UCArguments::buildExpectedArguments(QList<UCArgument*> declaredArguments)
{
    QHash<QString, QStringList> expectedArguments;

    Q_FOREACH (UCArgument* argument, declaredArguments) {
        expectedArguments.insert(argument->name(), argument->valueNames());
    }

    return expectedArguments;
}

QHash<QString, QStringList> UCArguments::parseRawArguments(QStringList rawArguments, QHash<QString, QStringList> expectedArguments)
{
    QHash<QString, QStringList> argumentsValues;
    QString name;
    QStringList values;

    QStringList::const_iterator i = rawArguments.constBegin();
    // ignore the first item in the rawArguments which is the name of the binary
    i++;
    for (; i != rawArguments.constEnd(); ++i) {
        QString rawArgument = (*i);
        if (rawArgument.startsWith('-')) {
            // it starts with a '-' therefore it is a named argument
            // remove all prepended '-'
            rawArgument = rawArgument.split('-', QString::SkipEmptyParts).join('-');
            // string after the '=' sign is a value for the argument
            values = rawArgument.split("=");
            name = values.takeAt(0);

            if (expectedArguments.contains(name)) {
                values.append(collectArgumentValues(i, rawArguments.constEnd(), expectedArguments[name].size()));
            } else {
                // unexpected named arguments are given at most one value
                values.append(collectArgumentValues(i, rawArguments.constEnd(), 1));
            }
            argumentsValues.insert(name, values);
        } else {
            // default/unnamed argument
            argumentsValues[""].append(rawArgument);
        }
    }

    return argumentsValues;
}

QStringList UCArguments::collectArgumentValues(QStringList::const_iterator& i, QStringList::const_iterator end, int nValues)
{
    QStringList values;
    for (; (i+1) != end; ++i) {
        QString rawArgument = *(i+1);
        if (rawArgument.startsWith('-')) {
            break;
        } else if (values.size() >= nValues) {
            break;
        } else {
            values.append(rawArgument);
        }
    }

    return values;
}

bool UCArguments::usageRequested(QStringList argumentNames) {
    return argumentNames.contains("help") ||
           argumentNames.contains("h") ||
           argumentNames.contains("usage");
}

bool UCArguments::requiredArgumentsProvided(QHash<QString, QStringList> argumentsValues, QString& error)
{
    // check if all required named arguments were passed along with their values
    Q_FOREACH (UCArgument* argument, m_arguments) {
        if (argument->required()) {
            if (!argumentsValues.contains(argument->name())) {
                UbuntuI18n* i18n = &UbuntuI18n::instance();
                error = i18n->tr("%1 is expecting an additional argument: %2");
                error = error.arg(m_applicationBinary).arg(argument->syntax());
                return false;
            } else if (argumentsValues[argument->name()].size() < argument->valueNames().size()) {
                UbuntuI18n* i18n = &UbuntuI18n::instance();
                error = i18n->tr("%1 is expecting a value for argument: %2");
                error = error.arg(m_applicationBinary).arg(argument->syntax());
                return false;
            }
        }
    }

    return true;
}

bool UCArguments::requiredDefaultArgumentProvided(QHash<QString, QStringList> argumentsValues, QString& error)
{
    // check if the required default argument was passed
    if (m_defaultArgument != NULL && m_defaultArgument->required() && !argumentsValues.contains("")) {
        UbuntuI18n* i18n = &UbuntuI18n::instance();
        error = i18n->tr("%1 is expecting additional arguments: %2").arg(m_applicationBinary).arg(m_defaultArgument->syntax());
        return false;
    } else {
        return true;
    }
}

void UCArguments::exposeArgumentsAsProperties(QHash<QString, QStringList> argumentsValues)
{
    QHash<QString, QStringList>::const_iterator i;

    for (i = argumentsValues.constBegin(); i != argumentsValues.constEnd(); ++i) {
        QString name = i.key();
        if (name.isEmpty()) {
            // ignore default argument
            continue;
        }

        // if the argument has no value, convert to bool
        // if the argument has a single value, convert to string
        // if the argument has multiple values, convert to a list of strings
        QStringList values = i.value();
        QVariant value;
        switch (values.size()) {
            case 0:
                value.setValue(true);
                break;
            case 1:
                value.setValue(values.at(0));
                break;
            default:
                value.setValue(values);
                break;
        }

        // verify that the property corresponding to the argument is defined in QML
        // and has a compatible type
        QQmlProperty qmlProperty(this, name, qmlContext(this));
        if (!qmlProperty.isValid()) {
            qWarning() << "Arguments: property " << value.typeName() << name << "needs to be defined";
            return;
        } else if (qmlProperty.propertyType() != QMetaType::QVariant
                   && !value.canConvert(qmlProperty.propertyType())) {
            qWarning() << "Arguments: property" << name << "is of incompatible type" << qmlProperty.propertyTypeName() << "but should be" << value.typeName();
            return;
        }

        // necessary for the value to be set to the QML property
        qmlProperty.write(value);
        // necessary for the value to be set to the C++ dynamic property
        const char* propertyName = name.toStdString().c_str();
        setProperty(propertyName, value);
    }
}
