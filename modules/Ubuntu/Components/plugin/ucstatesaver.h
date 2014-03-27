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

#ifndef UCSTATESAVER_H
#define UCSTATESAVER_H

#include <QtCore/QObject>
#include <QtQml/qqml.h>

class UCStateSaverAttachedPrivate;
class UCStateSaverAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString properties READ properties WRITE setProperties NOTIFY propertiesChanged)
public:
    UCStateSaverAttached(QObject *attachee);
    ~UCStateSaverAttached();

    // getter/setter
    bool enabled() const;
    void setEnabled(bool v);
    QString properties() const;
    void setProperties(const QString &list);

Q_SIGNALS:
    void enabledChanged();
    void propertiesChanged();

private:
    Q_DECLARE_PRIVATE(UCStateSaverAttached)
    QScopedPointer<UCStateSaverAttachedPrivate> d_ptr;
    Q_PRIVATE_SLOT(d_func(), void _q_init())
    Q_PRIVATE_SLOT(d_func(), void _q_save())
    Q_PRIVATE_SLOT(d_func(), void _q_globalEnableChanged(bool))
};

class UCStateSaver : public QObject
{
    Q_OBJECT
public:
    static UCStateSaverAttached *qmlAttachedProperties(QObject *attachee)
    {
        return new UCStateSaverAttached(attachee);
    }
};

QML_DECLARE_TYPEINFO(UCStateSaver, QML_HAS_ATTACHED_PROPERTIES)

#endif // UCSTATESAVER_H
