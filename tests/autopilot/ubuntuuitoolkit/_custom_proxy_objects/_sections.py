# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2016 Canonical Ltd.
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

import logging

from autopilot import logging as autopilot_logging
from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)


class Sections(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Sections Autopilot custom proxy object."""

    def __init__(self, *args):
        super().__init__(*args)
        self.listview = self.select_single(objectName='sections_listview')

    @autopilot_logging.log_action(logger.info)
    def click_section_button(self, section_index):
        """Click a section button of the Sections.

        :parameter section_index: The index of the section to click.
        :raise ToolkitException: If there is no section button with that index.

        """
        button_object_name = 'section_button_' + str(section_index)
        try:
            self.listview.click_element(button_object_name)
        except _common.ToolkitException:
            raise _common.ToolkitException(
                'Button with section index ' + str(section_index) +
                ' not found in Sections.')
