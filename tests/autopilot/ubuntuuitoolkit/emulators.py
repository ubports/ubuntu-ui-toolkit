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

"""Emulators was the old name for the custom proxy objects."""

import logging


logger = logging.getLogger(__name__)


logger.warning(
    'The ubuntuuitoolkit.emulators module is deprecated. Import the autopilot '
    'helpers from the top-level ubuntuuitoolkit module.')


__all__ = [
    'check_autopilot_version',
    'get_keyboard',
    'get_pointing_device',
    'TextInputPopover',
    'ActionSelectionPopover',
    'Base',
    'CheckBox',
    'ComposerSheet',
    'Empty',
    'UCListItem',
    'Header',
    'Dialog',
    'ItemSelector',
    'MainView',
    'MultiValue',
    'OptionSelector',
    'QQuickFlickable',
    'QQuickListView',
    'SingleControl',
    'SingleValue',
    'Standard',
    'Subtitled',
    'TabBar',
    'Tabs',
    'TextField',
    'Toolbar',
    'ToolkitEmulatorException',
    'UbuntuUIToolkitEmulatorBase',
]


from ubuntuuitoolkit._custom_proxy_objects import (
    check_autopilot_version,
    get_keyboard,
    get_pointing_device,
    CheckBox,
    Header,
    Dialog,
    UCListItem,
    MainView,
    OptionSelector,
    QQuickFlickable,
    QQuickListView,
    TabBar,
    Tabs,
    TextField,
    Toolbar,
    ToolkitException as ToolkitEmulatorException,
    UbuntuUIToolkitCustomProxyObjectBase as UbuntuUIToolkitEmulatorBase,
)
from ubuntuuitoolkit._custom_proxy_objects.listitems import (
    Base,
    Empty,
    ItemSelector,
    MultiValue,
    SingleControl,
    SingleValue,
    Standard,
    Subtitled
)
from ubuntuuitoolkit._custom_proxy_objects.popups import (
    TextInputPopover,
    ActionSelectionPopover,
    ComposerSheet
)
