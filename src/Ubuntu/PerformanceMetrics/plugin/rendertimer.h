/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef RENDERTIMER_H
#define RENDERTIMER_H

#include <QtCore/QtGlobal>
#include <QtCore/QObject>
#include <QtCore/QElapsedTimer>
#include <QtGui/qopenglfunctions.h>

class RenderTimerPrivate;

class RenderTimer : public QObject
{
    Q_OBJECT

    Q_ENUMS(TimerType)

public:
    enum TimerType {
        Automatic,
        Trivial,
#if defined(QT_OPENGL_ES)
        KHRFence,
        NVFence,
#else
        ARBTimerQuery,
        EXTTimerQuery
#endif
    };

    explicit RenderTimer(QObject* parent = 0);
    virtual ~RenderTimer();

    static bool isAvailable(TimerType type);
    static TimerType optimalTimerType();
    void setup(TimerType type);
    void teardown();
    void start();
    qint64 stop();

protected:
    void teardown(TimerType type);

private:
    Q_DECLARE_PRIVATE(RenderTimer)
    RenderTimerPrivate *d_ptr;
};

#endif // RENDERTIMER_H
