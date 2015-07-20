# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

"""Ubuntu UI Toolkit Autopilot tests and helpers."""


__all__ = [
    'AppHeader',
    'base',
    'ActionBar',
    'check_autopilot_version',
    'CheckBox',
    'environment',
    'fixture_setup',
    'get_keyboard',
    'get_pointing_device',
    'Header',
    'Dialog',
    'listitems',
    'UCListItem',
    'MainView',
    'OptionSelector',
    'pickers',
    'popups',
    'QQuickFlickable',
    'QQuickGridView',
    'QQuickListView',
    'Sections',
    'TabBar',
    'Tabs',
    'tests',
    'TextArea',
    'TextField',
    'Toolbar',
    'ToolkitException',
    'ubuntu_scenarios',
    'UbuntuListView11',
    'UbuntuUIToolkitCustomProxyObjectBase',
]


from ubuntuuitoolkit import (
    base,
    environment,
    fixture_setup,
    tests,
    ubuntu_scenarios
)
from ubuntuuitoolkit._custom_proxy_objects import (
    ActionBar,
    AppHeader,
    check_autopilot_version,
    CheckBox,
    get_keyboard,
    get_pointing_device,
    Header,
    Dialog,
    listitems,
    UCListItem,
    MainView,
    OptionSelector,
    pickers,
    popups,
    QQuickFlickable,
    QQuickGridView,
    QQuickListView,
    Sections,
    TabBar,
    Tabs,
    TextArea,
    TextField,
    Toolbar,
    ToolkitException,
    UbuntuUIToolkitCustomProxyObjectBase,
    UbuntuListView11,
)
