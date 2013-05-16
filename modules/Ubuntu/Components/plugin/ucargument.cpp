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

UCArgument::UCArgument(QObject *parent) :
    QObject(parent),
    m_required(true)
{
}

QString UCArgument::name() const
{
    return m_name;
}

void UCArgument::setName(QString name)
{
    // FIXME: reparse everything
    m_name = name;
    Q_EMIT(nameChanged());
}

QString UCArgument::help() const
{
    return m_help;
}

void UCArgument::setHelp(QString help)
{
    m_help = help;
    Q_EMIT(helpChanged());
}

bool UCArgument::required() const
{
    return m_required;
}

void UCArgument::setRequired(bool required)
{
    // FIXME: reparse everything
    m_required = required;
    Q_EMIT(requiredChanged());
}

QStringList UCArgument::valueNames() const
{
    return m_valueNames;
}

void UCArgument::setValueNames(QStringList valueNames)
{
    // FIXME: reparse everything
    m_valueNames = valueNames;
    Q_EMIT(valueNamesChanged());
}

QString UCArgument::syntax() const
{
    QString syntax;

    if (!m_name.isEmpty()) {
        syntax.append("--");
        syntax.append(m_name);

        if (!m_valueNames.empty()) {
            syntax.append('=');
            Q_FOREACH (QString valueName, m_valueNames) {
                syntax.append(valueName);
                if (valueName != m_valueNames.last()) {
                    syntax.append(' ');
                }
            }
        }
    } else {
        if (!m_valueNames.empty()) {
            Q_FOREACH (QString valueName, m_valueNames) {
                if (!m_required) {
                    valueName.prepend('[').append(']');
                }
                syntax.append(valueName);
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
