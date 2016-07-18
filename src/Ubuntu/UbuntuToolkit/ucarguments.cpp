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

#include "ucarguments_p.h"
#include "i18n_p.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QFileInfo>
#include <QtQml/QQmlProperty>
#include <QtQml/qqml.h>

UT_NAMESPACE_BEGIN

/*!
 * \qmltype Arguments
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu-commandline
 * \brief The Arguments class provides a way to declare what command line
 *        parameters are expected by the application.
 *
 * Example of use:
 *
 * \qml
 * Arguments {
 *     id: args
 *
 *     defaultArgument.help: "Expects URL of the media to play."
 *     defaultArgument.valueNames: ["URL"]
 *
 *     Argument {
 *         name: "fullscreen"
 *         help: "whether or not the media will be played fullscreen"
 *         required: false
 *     }
 *
 *     Argument {
 *         name: "volume"
 *         help: "level to which to set the volume"
 *         required: false
 *         valueNames: ["VOLUME_LEVEL"]
 *     }
 * }
 * \endqml
 *
 * The code above ensures that at least one argument is passed on the command
 * line which will then be available in QML via the defaultArgument property.
 * Optionally two other arguments can be passed:
 *
 * \e --fullscreen that will be exposed as a boolean property: \l{values}.fullscreen
 *
 * \e --volume that will be exposed as a string property: \l{values}.volume
 *
 * \qml
 * Item {
 *     Label {
 *         text: "volume = " + args.values.volume
 *     }
 *
 *     Label {
 *         text: "fullscreen = " + args.values.fullscreen
 *     }
 *
 *     Label {
 *         text: "URL = " + args.defaultArgument.at(0)
 *     }
 * }
 * \endqml
 *
 * If the application is launched with the incorrect arguments or with the
 * \e --usage argument an help will be outputted on the command line and the
 * application will exit:
 *
 * \code
 * Usage: application [--fullscreen] [--volume=VOLUME_LEVEL] URL
 * Options:
 * --fullscreen         whether or not the media will be played fullscreen
 * --volume=VOLUME_LEVE level to which to set the volume
 *
 * Expects URL of the media to play.
 * \endcode
 *
 * \sa Argument
 */

UCArguments::UCArguments(QObject *parent) :
    QObject(parent),
    m_completed(false),
    m_defaultArgument(NULL),
    m_values(new QQmlPropertyMap(this)),
    m_error(false)
{
    m_rawArguments = QCoreApplication::arguments();
    m_applicationBinary = QFileInfo(m_rawArguments[0]).fileName();
    m_defaultArgument = new UCArgument(this);
}

/*!
 * \qmlproperty Argument Arguments::defaultArgument
 *
 * The default argument corresponds to the values passed on the command line
 * without any name.
 *
 * For example in:
 *
 * \code
 * ./application --volume=42 http://myaddress
 * \endcode
 *
 * \e{http://myaddress} is the first value of the default argument.
 *
 */
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

/*!
 * \qmlproperty Object Arguments::values
 * \readonly
 *
 * Object of which properties have the values of the command line arguments.
 *
 */
QQmlPropertyMap* UCArguments::values() const
{
    return m_values;
}

/*!
 * \qmlproperty bool Arguments::error
 * \readonly
 *
 * Whether or not the command line arguments passed to the application correspond
 * to the expected arguments.
 *
 * Also set when calling \l quitWithError
 */
bool UCArguments::error() const
{
    return m_error;
}

/*!
 * \qmlproperty string Arguments::errorMessage
 * \readonly
 *
 * Error message describing how the command line arguments passed to the application
 * failed to correspond to the expected arguments.
 *
 * Also set when calling \l quitWithError
 *
 */
QString UCArguments::errorMessage() const
{
    return m_errorMessage;
}

QString UCArguments::usage() {
    QLatin1String indentation("  ");
    QString usage;
    QTextStream usageStream(&usage);
    UbuntuI18n* i18n = UbuntuI18n::instance();

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

    return usage;
}

void UCArguments::setErrorMessage(const QString& errorMessage)
{
    m_error = true;
    m_errorMessage = errorMessage;
    Q_EMIT errorChanged();
    Q_EMIT errorMessageChanged();
}

void UCArguments::quitApplication()
{
    /* This function can be called multiple times before the application actually
       quit. See comment at the end about QCoreApplication::quit().
    */
    static bool quitting = false;
    if (quitting) {
        return;
    }

    /* Call QCoreApplication::quit() when the event loop starts running.
       "If the event loop is not running, [QCoreApplication::quit()] does nothing."
       Ref.: http://qt-project.org/doc/qt-5.0/qtcore/qcoreapplication.html#exit
    */
    quitting = true;
    QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);
}

/*!
 * \qmlmethod void Arguments::printUsage()
 *
 * Outputs help to the console on how to pass arguments to the application.
 *
 */
void UCArguments::printUsage()
{
    qWarning() << qPrintable(usage());
}

/*!
 * \qmlmethod void Arguments::quitWithError(string errorMessage)
 *
 * Exits the application outputting \a errorMessage to the console.
 *
 */
void UCArguments::quitWithError(const QString& errorMessage)
{
    setErrorMessage(errorMessage);

    // output to the console, first the errorMessage if any then the usage
    if (!errorMessage.isEmpty()) {
        qWarning() << qPrintable(errorMessage);
    }
    quitApplication();
}

void UCArguments::classBegin()
{
}

void UCArguments::componentComplete()
{
    m_completed = true;
    parseAndExposeArguments();

    // changing attributes' name, required and valueNames dynamically is not supported
    Q_FOREACH (UCArgument* argument, m_arguments) {
        QObject::connect(argument, &UCArgument::nameChanged, this, &UCArguments::warningArgumentPropertyChanged);
        QObject::connect(argument, &UCArgument::requiredChanged, this, &UCArguments::warningArgumentPropertyChanged);
        QObject::connect(argument, &UCArgument::valueNamesChanged, this, &UCArguments::warningArgumentPropertyChanged);
    }
    QObject::connect(m_defaultArgument, &UCArgument::nameChanged, this, &UCArguments::warningArgumentPropertyChanged);
    QObject::connect(m_defaultArgument, &UCArgument::requiredChanged, this, &UCArguments::warningArgumentPropertyChanged);
    QObject::connect(m_defaultArgument, &UCArgument::valueNamesChanged, this, &UCArguments::warningArgumentPropertyChanged);
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


/*!
 * \qmlproperty list<Argument> Arguments::arguments
 * \default
 * \readonly
 *
 * List of command line \l{Argument}{arguments} that are expected by the application.
 *
 * See detailed description above for an example on how to use it.
 *
 */
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
        quitWithError();
        printUsage();
    }

    QString error;
    if (!requiredArgumentsProvided(argumentsValues, error)) {
        quitWithError(error);
        printUsage();
    }

    if (!requiredDefaultArgumentProvided(argumentsValues, error)) {
        quitWithError(error);
        printUsage();
    }

    setValuesOnArguments(argumentsValues);
    exposeArgumentsAsProperties(argumentsValues);
}

void UCArguments::setValuesOnArguments(const QHash<QString, QStringList>& argumentsValues)
{
    // pass the values to the arguments
    Q_FOREACH (UCArgument* argument, m_arguments) {
        QString name = argument->name();
        if (argumentsValues.contains(name)) {
            argument->setValues(argumentsValues.value(name));
        }
    }

    // pass the values to the defaultArgument
    if (m_defaultArgument != NULL) {
        if (argumentsValues.contains("")) {
            int numberArguments = m_defaultArgument->valueNames().size();
            m_defaultArgument->setValues(argumentsValues.value("").mid(0, numberArguments));
            if (numberArguments > 0) {
                // FIXME: not very elegant way to inform that values have changed
                Q_EMIT(defaultArgumentChanged());
            }
        }
    }
}

QHash<QString, QStringList> UCArguments::buildExpectedArguments(const QList<UCArgument*>& declaredArguments)
{
    QHash<QString, QStringList> expectedArguments;

    Q_FOREACH (UCArgument* argument, declaredArguments) {
        expectedArguments.insert(argument->name(), argument->valueNames());
    }

    return expectedArguments;
}

QHash<QString, QStringList> UCArguments::parseRawArguments(const QStringList& rawArguments, const QHash<QString, QStringList>& expectedArguments)
{
    QHash<QString, QStringList> argumentsValues;
    QString name;
    QStringList values;

    if (rawArguments.size() == 0) {
        return argumentsValues;
    }

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
                values.append(collectArgumentValues(i, rawArguments.constEnd(), expectedArguments.value(name).size() - values.size()));
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

    if (i == end) {
        return values;
    }

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

bool UCArguments::usageRequested(const QStringList& argumentNames) {
    return argumentNames.contains("help") ||
           argumentNames.contains("h") ||
           argumentNames.contains("usage");
}

bool UCArguments::requiredArgumentsProvided(const QHash<QString, QStringList>& argumentsValues, QString& error)
{
    // check if all required named arguments were passed along with their values
    Q_FOREACH (UCArgument* argument, m_arguments) {
        if (argument->required()) {
            if (!argumentsValues.contains(argument->name())) {
                UbuntuI18n* i18n = UbuntuI18n::instance();
                error = i18n->tr("%1 is expecting an additional argument: %2");
                error = error.arg(m_applicationBinary).arg(argument->syntax());
                return false;
            } else if (argumentsValues.value(argument->name()).size() < argument->valueNames().size()) {
                UbuntuI18n* i18n = UbuntuI18n::instance();
                error = i18n->tr("%1 is expecting a value for argument: %2");
                error = error.arg(m_applicationBinary).arg(argument->syntax());
                return false;
            }
        }
    }

    return true;
}

bool UCArguments::requiredDefaultArgumentProvided(const QHash<QString, QStringList>& argumentsValues, QString& error)
{
    // check if the required default argument was passed
    if (m_defaultArgument != NULL && m_defaultArgument->required() &&
         ((!argumentsValues.contains("") && m_defaultArgument->valueNames().size() > 0)
       || (argumentsValues.value("").size() < m_defaultArgument->valueNames().size()))) {
        UbuntuI18n* i18n = UbuntuI18n::instance();
        error = i18n->tr("%1 is expecting additional arguments: %2").arg(m_applicationBinary).arg(m_defaultArgument->syntax());
        return false;
    } else {
        return true;
    }
}

void UCArguments::exposeArgumentsAsProperties(const QHash<QString, QStringList>& argumentsValues)
{
    Q_FOREACH (UCArgument* argument, m_arguments) {
        QString name = argument->name();
        // if the argument has no value, convert to bool
        // if the argument has a single value, convert to string
        // if the argument has multiple values, convert to a list of strings
        QStringList values = argument->values();
        QVariant value;
        switch (argument->valueNames().size()) {
            case 0:
                if (argumentsValues.contains(name)) {
                    value.setValue(true);
                } else {
                    // case of a boolean argument that was not passed on the command line
                    value.setValue(false);
                }
                break;
            case 1:
                if (values.size() > 0) {
                    value.setValue(values.at(0));
                }
                break;
            default:
                value.setValue(values);
                break;
        }
        m_values->insert(name, value);
        Q_EMIT m_values->valueChanged(name, value);
    }
}

void UCArguments::warningArgumentPropertyChanged()
{
    qWarning() << "Changing properties of arguments dynamically is not supported.";
}

UT_NAMESPACE_END
