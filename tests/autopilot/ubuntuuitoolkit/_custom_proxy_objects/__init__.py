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
    'AppHeader',
    'ActionBar',
    'ActionSelectionPopover',
    'check_autopilot_version',
    'CheckBox',
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
    'TextArea',
    'TextField',
    'Toolbar',
    'ToolkitException',
    'UbuntuListView11',
    'UbuntuListView',
    'UbuntuUIToolkitCustomProxyObjectBase',
]

from ubuntuuitoolkit._custom_proxy_objects._actionbar import ActionBar
from ubuntuuitoolkit._custom_proxy_objects._sections import Sections
from ubuntuuitoolkit._custom_proxy_objects._checkbox import CheckBox
from ubuntuuitoolkit._custom_proxy_objects._common import (
    check_autopilot_version,
    get_keyboard,
    get_pointing_device,
    ToolkitException,
    UbuntuUIToolkitCustomProxyObjectBase,
)
from ubuntuuitoolkit._custom_proxy_objects._flickable import QQuickFlickable
from ubuntuuitoolkit._custom_proxy_objects._header import (
    AppHeader,
    Header,
)

from ubuntuuitoolkit._custom_proxy_objects.popups import (
    Dialog,
    ActionSelectionPopover,
)

from ubuntuuitoolkit._custom_proxy_objects import listitems
from ubuntuuitoolkit._custom_proxy_objects._uclistitem import (
    UCListItem
)
from ubuntuuitoolkit._custom_proxy_objects._mainview import MainView
from ubuntuuitoolkit._custom_proxy_objects._optionselector import (
    OptionSelector
)
from ubuntuuitoolkit._custom_proxy_objects import pickers
from ubuntuuitoolkit._custom_proxy_objects import popups
from ubuntuuitoolkit._custom_proxy_objects._qquickgridview import (
    QQuickGridView
)
from ubuntuuitoolkit._custom_proxy_objects._qquicklistview import (
    QQuickListView
)
from ubuntuuitoolkit._custom_proxy_objects._tabbar import TabBar
from ubuntuuitoolkit._custom_proxy_objects._tabs import Tabs
from ubuntuuitoolkit._custom_proxy_objects._textarea import TextArea
from ubuntuuitoolkit._custom_proxy_objects._textfield import TextField
from ubuntuuitoolkit._custom_proxy_objects._toolbar import Toolbar
from ubuntuuitoolkit._custom_proxy_objects._ubuntulistview import (
    UbuntuListView11,
    UbuntuListView,
)
