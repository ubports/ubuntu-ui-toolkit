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

#include "ucargument.h"

UT_NAMESPACE_BEGIN
/*!
 * \qmltype Argument
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu-commandline
 * \brief The Argument class specifies what type a given command line parameter should be.
 *
 * \b Important: Argument is not used on its own but must be a child of \l Arguments.
 *
 * An Argument is defined by its \l name and \l valueNames properties which correspond
 * to a command line parameter (e.g. \e{--target}) and the values passed to it
 * (e.g. \e{--target=http://myaddress}).
 *
 * For example, the following code allows to retrieve parameters on the
 * command line of the form \e{--target=URL}:
 *
 * \qml
 * Argument {
 *     name: "target"
 *     help: "address of the resource to retrieve"
 *     required: true
 *     valueNames: ["URL"]
 * }
 * \endqml
 *
 * To retrieve a value passed to the parameter knowing its position, use the \l at
 * method.
 *
 * \sa Arguments
 *
 */

UCArgument::UCArgument(QObject *parent) :
    QObject(parent),
    m_required(true)
{
}

/*!
 * \qmlproperty string Argument::name
 *
 * Name identifying the command line argument.
 *
 * For example 'target' corrsesponds to the \e{'--target'} command line argument.
 *
 */
QString UCArgument::name() const
{
    return m_name;
}

void UCArgument::setName(const QString &name)
{
    m_name = name;
    Q_EMIT(nameChanged());
}

/*!
 * \qmlproperty string Argument::help
 *
 * Help displayed when launching the application with --usage.
 *
 */
QString UCArgument::help() const
{
    return m_help;
}

void UCArgument::setHelp(const QString &help)
{
    m_help = help;
    Q_EMIT(helpChanged());
}

/*!
 * \qmlproperty bool Argument::required
 *
 * Whether or not this argument needs to be passed on the command line when
 * launching the application.
 *
 */
bool UCArgument::required() const
{
    return m_required;
}

void UCArgument::setRequired(bool required)
{
    m_required = required;
    Q_EMIT(requiredChanged());
}

/*!
 * \qmlproperty list<string> Argument::valueNames
 *
 * Names given to the values of this argument. They are used when displaying the
 * expected command line arguments of the application.
 *
 * Typically all capital letters (e.g. URL).
 *
 */
QStringList UCArgument::valueNames() const
{
    return m_valueNames;
}

void UCArgument::setValueNames(const QStringList &valueNames)
{
    m_valueNames = valueNames;
    Q_EMIT(valueNamesChanged());
}

QString UCArgument::syntax() const
{
    QString syntax;

    if (!m_name.isEmpty()) {
        // for named argument: --name=value1 value2
        syntax.append("--");
        syntax.append(m_name);

        if (!m_valueNames.empty()) {
            syntax.append('=');
            Q_FOREACH (const QString &valueName, m_valueNames) {
                syntax.append(valueName);
                if (valueName != m_valueNames.last()) {
                    syntax.append(' ');
                }
            }
        }
    } else {
        // for unnamed required argument: value1 value2
        // for unnamed optional argument: [value1] [value2]
        if (!m_valueNames.empty()) {
            Q_FOREACH (const QString &valueName, m_valueNames) {
                QString outputName = valueName;
                if (!m_required) {
                    outputName.prepend('[').append(']');
                }
                syntax.append(outputName);
                if (valueName != m_valueNames.last()) {
                    syntax.append(' ');
                }
            }
        }
    }
    return syntax;
}

QString UCArgument::usage() const
{
    // ensure that the help string for the arguments are aligned vertically
    QString usage;
    usage.append(syntax().leftJustified(20, ' ', true));
    usage.append(' ').append(m_help);
    return usage;
}

QStringList UCArgument::values() const
{
    return m_values;
}

void UCArgument::setValues(const QStringList &values)
{
    m_values = values;
}

/*!
 * \qmlmethod string Argument::at(int i)
 *
 * Returns the \e{i}th value of the argument.
 * Values are counted from 0.
 *
 */
QVariant UCArgument::at(int i) const
{
    if (i >= 0 && i < m_values.size()) {
        return QVariant(m_values.at(i));
    } else {
        return QVariant();
    }
}

UT_NAMESPACE_END
