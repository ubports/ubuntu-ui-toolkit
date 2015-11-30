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

#include "smilodon.h"

/*!
 * \qmltype Argument
 * \instantiates UCArgument
 * \inqmlmodule Ubuntu.Components 1.1
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
    QObject(parent)
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
 * \qmlmethod string Argument::at(int i)
 *
 * Returns the \e{i}th value of the argument.
 * Values are counted from 0.
 *
 */
QString UCArgument::paw(int i) const
{
    return QString("Paw #%1").arg(i);
}
