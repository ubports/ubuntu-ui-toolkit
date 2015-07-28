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

#ifndef UCARGUMENT_H
#define UCARGUMENT_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

class UCArgument : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString help READ help WRITE setHelp NOTIFY helpChanged)
    Q_PROPERTY(bool required READ required WRITE setRequired NOTIFY requiredChanged)
    Q_PROPERTY(QStringList valueNames READ valueNames WRITE setValueNames NOTIFY valueNamesChanged)

public:
    explicit UCArgument(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);
    QString help() const;
    void setHelp(const QString &help);
    bool required() const;
    void setRequired(bool required);
    QStringList valueNames() const;
    void setValueNames(const QStringList &valueNames);

    // FIXME: maybe define UCArgument as a QQmlListProperty instead so that
    // operator[] is available from QML
    Q_INVOKABLE QVariant at(int i) const;

protected:
    QString syntax() const;
    QString usage() const;
    QStringList values() const;
    void setValues(const QStringList &values);

Q_SIGNALS:
    void nameChanged();
    void helpChanged();
    void requiredChanged();
    void valueNamesChanged();

private:
    QString m_name;
    QString m_help;
    bool m_required;
    QStringList m_valueNames;
    QStringList m_values;

    friend class UCArguments;
};

#endif // UCARGUMENT_H
