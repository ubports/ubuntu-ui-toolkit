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
 */

#ifndef UCHAPTICS_H
#define UCHAPTICS_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtQml/QQmlEngine>

class UCHaptics : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)
    Q_PROPERTY(QObject *effect READ effect CONSTANT)
public:
    explicit UCHaptics(QObject *parent = 0);

    bool enabled() const;
    QObject *effect() const;

Q_SIGNALS:
    void enabledChanged();

public Q_SLOTS:
    void play(const QVariant &customEffect = QVariant());
};

class QQmlEngine;
class HapticsProxy : public QObject
{
    Q_OBJECT
public:
    explicit HapticsProxy(QObject *parent = 0)
        : QObject(parent)
        , m_proxyObject(Q_NULLPTR)
        , m_engine(static_cast<QQmlEngine*>(parent))
    {
        if (!m_engine) {
            qFatal("HaptixProxy must be a child of the QML Engine!");
        }
    }
    ~HapticsProxy()
    {
        m_instance = Q_NULLPTR;
    }

    static HapticsProxy *instance(QQmlEngine *engine = Q_NULLPTR)
    {
        if (!m_instance) {
            m_instance = new HapticsProxy(engine);
        }
        return m_instance;
    }

    void initialize();

    bool enabled();
    QObject *effect();
    void play(const QVariant &customEffect);

Q_SIGNALS:
    void enabledChanged();

private:
    static HapticsProxy *m_instance;
    QObject *m_proxyObject;
    QQmlEngine *m_engine;
};

#endif // UCHAPTICS_H
