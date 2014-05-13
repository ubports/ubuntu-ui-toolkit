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

#include "ulplugin.h"
#include "ullayouts.h"
#include "ulconditionallayout.h"
#include "ulitemlayout.h"

#include <qqml.h>

void PluginPlugin::registerTypeVersions(const char *uri, int major, int minor)
{
    qmlRegisterType<ULLayouts>(uri, major, minor, "Layouts");
    qmlRegisterType<ULConditionalLayout>(uri, major, minor, "ConditionalLayout");
    qmlRegisterType<ULItemLayout>(uri, major, minor, "ItemLayout");
}

void PluginPlugin::registerTypes(const char *uri)
{
    // @uri Ubuntu.Layouts
    registerTypeVersions(uri, 0, 1);
    registerTypeVersions(uri, 1, 0);
}
