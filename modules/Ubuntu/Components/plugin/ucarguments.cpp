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

UCArguments::UCArguments(QObject *parent) :
    QObject(parent),
    m_defaultArgument(NULL)
{
}

UCArgument* UCArguments::defaultArgument() const
{
    return m_defaultArgument;
}

void UCArguments::setDefaultArgument(UCArgument* argument)
{
    // FIXME
    m_defaultArgument = argument;
    Q_EMIT(defaultArgumentChanged());
}

QQmlListProperty<UCArgument> UCArguments::arguments()
{
    /* FIXME: should be const
     *
     * From: http://qt-project.org/doc/qt-5.0/qtqml/qqmllistproperty.html
     *
     * Generally this constructor should not be used in production code,
     * as a writable QList violates QML's memory management rules. However,
     * this constructor can very useful while prototyping.
     */
    return QQmlListProperty<UCArgument>(this, m_arguments);
}

void UCArguments::quitAndPrintUsage(QString errorMessage)
{
    // FIXME
}
