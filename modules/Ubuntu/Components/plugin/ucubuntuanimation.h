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
 */

#ifndef UCUBUNTUANIMATION_H
#define UCUBUNTUANIMATION_H

#include <QObject>
#include <QtCore/QDebug>
#include <QtCore/QEasingCurve>

class UCUbuntuAnimation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int SnapDuration READ SnapDuration CONSTANT)
    Q_PROPERTY(int FastDuration READ FastDuration CONSTANT)
    Q_PROPERTY(int SlowDuration READ SlowDuration CONSTANT)
    Q_PROPERTY(int SleepyDuration READ SleepyDuration CONSTANT)
    Q_PROPERTY(QEasingCurve StandardEasing READ StandardEasing CONSTANT)
    Q_PROPERTY(QEasingCurve StandardEasingIn READ StandardEasingIn CONSTANT)
    Q_PROPERTY(QEasingCurve StandardEasingOut READ StandardEasingOut CONSTANT)

public:
    explicit UCUbuntuAnimation(QObject *parent = 0);
    
    int SnapDuration() const { return 125; }
    int FastDuration() const { return 250; }
    int SlowDuration() const { return 500; }
    int SleepyDuration() const { return 1000; }
    const QEasingCurve& StandardEasing() const {
        qWarning() << "Use of UbuntuAnimation.StandardEasing is deprecated. Use UbuntuAnimation.StandardEasingOutOut instead.";
        return m_standardEasingOut;
    }
    const QEasingCurve& StandardEasingIn() const {
        return m_standardEasingIn;
    }
    const QEasingCurve& StandardEasingOut() const {
        return m_standardEasingOut;
    }

private:
    QEasingCurve m_standardEasingIn;
    QEasingCurve m_standardEasingOut;
};

/*
 * Registration function used by qmlRegisterSingletonType
 */
class QQmlEngine;
class QJSEngine;

static QObject *registerUCUbuntuAnimation(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    UCUbuntuAnimation *animation = new UCUbuntuAnimation();
    return animation;
}

#endif // UCUBUNTUANIMATION_H
