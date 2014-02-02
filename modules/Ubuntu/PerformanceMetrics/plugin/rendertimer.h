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

class RenderTimer
{
public:
    RenderTimer();
    virtual ~RenderTimer();

    virtual bool isAvailable() = 0;
    virtual void setup();
    virtual void teardown();
    virtual void start() = 0;
    virtual qint64 stop() = 0;
};

#endif // RENDERTIMER_H
