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

"""Ubuntu UI Toolkit Autopilot custom proxy objects."""


__all__ = [
    'ActionSelectionPopover',
    'Base',
    'check_autopilot_version',
    'CheckBox',
    'ComposerSheet',
    'Empty',
    'Flickable',
    'get_keyboard',
    'get_pointing_device',
    'Header',
    'ItemSelector',
    'MainView',
    'MultiValue',
    'OptionSelector',
    'QQuickListView',
    'SingleControl',
    'SingleValue',
    'Standard',
    'Subtitled',
    'TabBar',
    'Tabs',
    'TextField',
    'Toolbar',
    'ToolkitException',
    'UbuntuUIToolkitCustomProxyObjectBase',
]

from ubuntuuitoolkit._custom_proxy_objects._checkbox import CheckBox
from ubuntuuitoolkit._custom_proxy_objects._common import (
    check_autopilot_version,
    get_keyboard,
    get_pointing_device,
    ToolkitException,
    UbuntuUIToolkitCustomProxyObjectBase,
)
from ubuntuuitoolkit._custom_proxy_objects._flickable import Flickable
from ubuntuuitoolkit._custom_proxy_objects._header import Header
from ubuntuuitoolkit._custom_proxy_objects._listitems import (
    Base,
    Empty,
    ItemSelector,
    MultiValue,
    SingleControl,
    SingleValue,
    Standard,
    Subtitled,
)
from ubuntuuitoolkit._custom_proxy_objects._mainview import MainView
from ubuntuuitoolkit._custom_proxy_objects._optionselector import (
    OptionSelector
)
from ubuntuuitoolkit._custom_proxy_objects._popups import (
    ActionSelectionPopover,
    ComposerSheet,
)
from ubuntuuitoolkit._custom_proxy_objects._qquicklistview import (
    QQuickListView
)
from ubuntuuitoolkit._custom_proxy_objects._tabbar import TabBar
from ubuntuuitoolkit._custom_proxy_objects._tabs import Tabs
from ubuntuuitoolkit._custom_proxy_objects._textfield import TextField
from ubuntuuitoolkit._custom_proxy_objects._toolbar import Toolbar
