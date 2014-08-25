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

import logging

from autopilot import logging as autopilot_logging
from autopilot.introspection import dbus

from ubuntuuitoolkit._custom_proxy_objects import _common, _flickable

logger = logging.getLogger(__name__)


class QQuickListView(_flickable.QQuickFlickable):

    @autopilot_logging.log_action(logger.info)
    def click_element(self, object_name, direction=None):
        """Click an element from the list.

        It swipes the element into view if it's center is not visible.

        :parameter objectName: The objectName property of the element to click.
        :parameter direction: The direction where the element is, it can be
            either 'above' or 'below'. Default value is None, which means we
            don't know where the object is and we will need to search the full
            list.

        """
        try:
            element = self.select_single(objectName=object_name)
        except dbus.StateNotFoundError:
            # The element might be on a part of the list that hasn't been
            # created yet. We have to search for it scrolling the entire list.
            element = self._find_element(object_name, direction)
        self.swipe_child_into_view(element)
        self.pointing_device.click_object(element)

    @autopilot_logging.log_action(logger.info)
    def _find_element(self, object_name, direction=None):
        if direction is None:
            # We don't know where the object is so we start looking for it from
            # the top.
            self.swipe_to_top()
            direction = 'below'

        if direction == 'below':
            fail_condition = lambda: self.atYEnd
            swipe_method = self.swipe_to_show_more_below
        elif direction == 'above':
            fail_condition = lambda: self.atYBeginning
            swipe_method = self.swipe_to_show_more_above
        else:
            raise _common.ToolkitException(
                'Invalid direction: {}'.format(direction))

        containers = self._get_containers()
        while not fail_condition():
            try:
                return self.select_single(objectName=object_name)
            except dbus.StateNotFoundError:
                swipe_method(containers)
        else:
            # We have reached the start or the end of the list.
            try:
                return self.select_single(objectName=object_name)
            except dbus.StateNotFoundError:
                raise _common.ToolkitException(
                    'List element with objectName "{}" not found.'.format(
                        object_name))

    def _is_element_clickable(self, object_name):
        child = self.select_single(objectName=object_name)
        containers = self._get_containers()
        return self._is_child_visible(child, containers)
