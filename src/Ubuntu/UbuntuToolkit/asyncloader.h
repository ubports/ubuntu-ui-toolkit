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

#ifndef ASYNCLOADER_H
#define ASYNCLOADER_H

#include <QtQml/QQmlIncubator>
#include <QtQml/QQmlComponent>
#include <ubuntutoolkitglobal.h>

class QQuickItem;
class QQmlContext;

namespace UbuntuToolkit {

class UBUNTUTOOLKIT_EXPORT AsyncLoader : public QObject, public QQmlIncubator
{
    Q_OBJECT
public:
    explicit AsyncLoader(QObject *parent = 0);
    ~AsyncLoader();

    void load(QQmlEngine *engine, const QUrl &url, QQmlContext *context = 0);
    void load(QQmlComponent *component, QQmlContext *context = 0);
    void cancel();
    void reset();

Q_SIGNALS:
    void statusChanged(Status status, QObject *object);

protected:
    void setInitialState(QObject *object) override;
    void statusChanged(Status status) override;

    void onComponentStatusChanged(QQmlComponent::Status status);
    void detachComponent();

    QQmlComponent *m_component;
    QQmlContext *m_context;
    QObject *m_object;
    bool m_ownComponent;
};

} // namespace UbuntuToolkit

#endif // ASYNCLOADER_H
