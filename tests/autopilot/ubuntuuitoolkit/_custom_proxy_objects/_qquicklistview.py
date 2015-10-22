# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014, 2015 Canonical Ltd.
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
import time

from autopilot import (
    input,
    logging as autopilot_logging,
    platform
)
from autopilot.introspection import dbus

from ubuntuuitoolkit._custom_proxy_objects import _common, _flickable

logger = logging.getLogger(__name__)


class QQuickListView(_flickable.QQuickFlickable):

    @autopilot_logging.log_action(logger.info)
    def click_element(self, object_name, direction=None):
        """Click an element from the list.

        It swipes the element into view if it's center is not visible.

        :parameter object_name: The objectName property of the element to
            click.
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
        return element

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

    @autopilot_logging.log_action(logger.info)
    def enable_select_mode(self):
        """Default implementation to enable select mode. Performs a long tap
           over the first list item in the ListView. The delegates must be
           the new ListItem components.
        """
        self.swipe_to_top()
        first_item = self._get_first_item()
        self.pointing_device.click_object(first_item, press_duration=2)
        try:
            self.wait_select_single('QQuickItem',
                                    objectName='selection_panel0')
        except dbus.StateNotFoundError:
            raise _common.ToolkitException(
                'ListView delegate is not a ListItem or not in selectMode')

    def _get_first_item(self):
        """Returns the first item from the ListView."""
        items = self.get_children_by_type('QQuickItem')[0].get_children()
        items = sorted(items, key=lambda item: item.globalRect.y)
        return items[0]

    @autopilot_logging.log_action(logger.info)
    def drag_item(self, from_index, to_index):
        if platform.model() != 'Desktop':
            raise NotImplementedError(
                'Drag does not work on the phone because of bug #1266601')

        self._enable_drag_mode()

        both_items_visible = (
            self._is_drag_panel_visible(from_index) and
            self._is_drag_panel_visible(to_index))
        if both_items_visible:
            self._drag_both_items_visible(from_index, to_index)
        else:
            self._drag_item_with_pagination(from_index, to_index)
        # wait 1 second till all animations complete
        time.sleep(1)

    @autopilot_logging.log_action(logger.debug)
    def _enable_drag_mode(self):
        self.swipe_to_top()
        first_item = self._get_first_item()
        self.pointing_device.click_object(first_item, press_duration=2)
        self._get_drag_panel(wait=True)

    def _is_drag_panel_visible(self, index):
        try:
            drag_panel = self._get_drag_panel(index)
        except:
            return False
        else:
            return self.is_child_visible(drag_panel)

    def _get_drag_panel(self, index=0, wait=False):
        select_method = self.wait_select_single if wait else self.select_single
        try:
            return select_method(
                'QQuickItem', objectName='drag_panel{}'.format(index))
        except ValueError:
            # While we are dragging, each panel item is duplicated.
            # A ValueError will be raised in this scenario. We can take
            # any of the returned elements, as they have the same position.
            return self.select_many(
                'QQuickItem', objectName='drag_panel{}'.format(index))[0]

    def _drag_both_items_visible(self, from_index, to_index):
        from_drag_handler = self._get_drag_panel(from_index)
        to_drag_handler = self._get_drag_panel(to_index)
        start_x, start_y = input.get_center_point(from_drag_handler)
        stop_x, stop_y = input.get_center_point(to_drag_handler)
        self.pointing_device.drag(start_x, start_y, stop_x, stop_y)

    def _drag_item_with_pagination(self, from_index, to_index):
        # the from_index might be invisible
        from_item = self._find_element('listitem{}'.format(from_index))
        from_item.swipe_into_view()
        from_drag_panel = self._get_drag_panel(from_index)
        containers = self._get_containers()
        if from_index < to_index:
            self._drag_downwards(from_drag_panel, to_index, containers)
        else:
            self._drag_upwards(from_drag_panel, to_index, containers)

    def _drag_downwards(self, drag_panel, to_index, containers):
        visible_bottom = _flickable._get_visible_container_bottom(
            containers)
        start_x, start_y = input.get_center_point(drag_panel)

        self.pointing_device.move(start_x, start_y)
        self.pointing_device.press()
        stop_x = start_x
        self.pointing_device.move(stop_x, visible_bottom)
        to_drag_panel = self._get_drag_panel(to_index, wait=True)
        while not self.is_child_visible(to_drag_panel):
            pass
        # stop moving
        h = to_drag_panel.height / 2
        self.pointing_device.move(stop_x, self.pointing_device.y - h)
        # move under the item with the to_index
        to_drag_panel = self._get_drag_panel(to_index - 1)
        stop_y = (
            to_drag_panel.globalRect.y +
            to_drag_panel.globalRect.height)
        self.pointing_device.move(stop_x, stop_y)
        self.pointing_device.release()

    def _drag_upwards(self, handler, to_index, containers):
        visible_top = _flickable._get_visible_container_top(
            containers)
        start_x, start_y = input.get_center_point(handler)

        self.pointing_device.move(start_x, start_y)
        self.pointing_device.press()
        stop_x = start_x
        # Header alters topMargin, therefore drag only till that edge
        self.pointing_device.move(stop_x, visible_top + self.topMargin)
        to_drag_panel = self._get_drag_panel(to_index, wait=True)
        while not self.is_child_visible(to_drag_panel):
            pass
        # stop moving
        h = to_drag_panel.height / 2
        self.pointing_device.move(stop_x, self.pointing_device.y + h)
        # move after the item with the to_index
        to_drag_panel = self._get_drag_panel(to_index)
        stop_y = (
            to_drag_panel.globalRect.y +
            to_drag_panel.globalRect.height)
        self.pointing_device.move(stop_x, stop_y)
        self.pointing_device.release()
