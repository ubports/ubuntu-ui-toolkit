/*
 * Copyright 2016 Canonical Ltd.
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
 */

#ifndef UCPAGEWRAPPERINCUBATOR_H
#define UCPAGEWRAPPERINCUBATOR_H

#include <QObject>
#include <QQmlIncubator>
#include <QJSValue>
#include <QVariant>

class UCPageWrapperIncubator : public QObject, public QQmlIncubator
{
    Q_OBJECT

    Q_PROPERTY(int status READ status)
    Q_PROPERTY(QJSValue onStatusChanged READ onStatusChanged WRITE setOnStatusChanged)
    Q_PROPERTY(QObject* object READ object)

public:
    UCPageWrapperIncubator(QQmlIncubator::IncubationMode mode = QQmlIncubator::Asynchronous,
                           QObject *parent = 0);
    ~UCPageWrapperIncubator();

    // QQmlIncubator interface
    virtual void statusChanged(Status status) override;
    Q_INVOKABLE void forceCompletion();

    QJSValue onStatusChanged() const;
    void setOnStatusChanged(QJSValue onStatusChanged);

protected:
    // QQmlIncubator interface
    virtual void setInitialState(QObject *target) override;


Q_SIGNALS:
    void statusHasChanged (QQmlIncubator::Status status);
    void initialStateRequested (QObject *target);
    void enterOnStatusChanged();

private:
    QJSValue m_onStatusChanged;
};

#endif // UCPAGEWRAPPERINCUBATOR_H
