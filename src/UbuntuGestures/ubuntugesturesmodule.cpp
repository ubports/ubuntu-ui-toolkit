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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include <QtQml/QQmlEngine>
#include <QtQml>
#include "ubuntugesturesmodule.h"
#include <private/ucswipearea_p.h>

UG_NAMESPACE_BEGIN

void UbuntuGesturesModule::defineModule(const char *uri)
{
    qmlRegisterType<UCSwipeArea>(uri, 1, 3, "SwipeArea");
}

void UbuntuGesturesModule::undefineModule()
{
    // nothing so far
}

UG_NAMESPACE_END
