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
#include <QtCore/QEasingCurve>

class UCUbuntuAnimation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float SnapDuration READ SnapDuration NOTIFY SnapDurationChanged)
    Q_PROPERTY(float FastDuration READ FastDuration NOTIFY FastDurationChanged)
    Q_PROPERTY(float SlowDuration READ SlowDuration NOTIFY SlowDurationChanged)
    Q_PROPERTY(float SleepyDuration READ SleepyDuration NOTIFY SleepyDurationChanged)
    Q_PROPERTY(QEasingCurve StandardEasing READ StandardEasing NOTIFY StandardEasingChanged)

public:
    explicit UCUbuntuAnimation(QObject *parent = 0);
    
    float SnapDuration() const { return 125.0f; }
    float FastDuration() const { return 250.0f; }
    float SlowDuration() const { return 500.0f; }
    float SleepyDuration() const { return 1000.0f; }
    QEasingCurve StandardEasing() const {
        static QEasingCurve standardEasing(QEasingCurve::OutQuint);
        return standardEasing;
    }

Q_SIGNALS:
    void SnapDurationChanged();
    void FastDurationChanged();
    void SlowDurationChanged();
    void SleepyDurationChanged();
    void StandardEasingChanged();
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
