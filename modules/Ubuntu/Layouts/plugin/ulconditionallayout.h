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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef ULCONDITIONALLAYOUT_H
#define ULCONDITIONALLAYOUT_H

#include <QtQml/QQmlComponent>
#include <QtCore/QStringList>

class ULConditionalLayoutAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList itemNames READ itemNames WRITE setItemNames NOTIFY itemNamesChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit ULConditionalLayoutAttached(QObject *parent = 0);

Q_SIGNALS:
    void itemNamesChanged();
    void nameChanged();

public: // getter/setter
    QStringList itemNames() const;
    void setItemNames(const QStringList &names);
    QString name() const;
    void setName(const QString &name);
};

class QQmlBinding;
class ULConditionalLayoutPrivate;
class ULConditionalLayout : public QQmlComponent
{
    Q_OBJECT
    Q_DISABLE_COPY(ULConditionalLayout)

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QQmlBinding* when READ when WRITE setWhen)

public:
    explicit ULConditionalLayout(QObject *parent = 0);
    ~ULConditionalLayout();

    static ULConditionalLayoutAttached * qmlAttachedProperties(QObject *owner);

private:
    QString name() const;
    void setName(const QString &name);
    QQmlBinding *when() const;
    void setWhen(QQmlBinding *when);

private:
    Q_DECLARE_PRIVATE(ULConditionalLayout)
    QScopedPointer<ULConditionalLayoutPrivate> d_ptr;
};

QML_DECLARE_TYPEINFO(ULConditionalLayout, QML_HAS_ATTACHED_PROPERTIES)

#endif // ULCONDITIONALLAYOUT_H
