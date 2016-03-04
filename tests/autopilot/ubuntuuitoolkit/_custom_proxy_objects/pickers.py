# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
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

import datetime
import logging

from autopilot import logging as autopilot_logging
from autopilot.introspection import dbus

from ubuntuuitoolkit._custom_proxy_objects import (
    _common,
    _qquicklistview
)


logger = logging.getLogger(__name__)


class DatePicker(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Autopilot helper for the DatePicker component."""

    @autopilot_logging.log_action(logger.info)
    def pick_date(self, date):
        """Pick a date from the date picker.

        :parameter date: The date to pick.
        :type date: An object with year, month and day attributes, like
            python's datetime.date.
        :raises ubuntuuitoolkit.ToolkitException if the mode of the picker
            doesn't let select a date.

        """
        if not self._is_date_picker():
            raise _common.ToolkitException(
                "Can't pick date. The picker mode is: {!r}.".format(self.mode))
        if 'Years' in self.mode:
            self._pick_year(date.year)
            self.year.wait_for(date.year)
        if 'Month' in self.mode:
            # Python's date object starts at one. The model in the date picker
            # at 0.
            self._pick_month(date.month - 1)
            self.month.wait_for(date.month - 1)
        if 'Day' in self.mode:
            self._pick_day(date.day)
            self.day.wait_for(date.day)

    def _is_date_picker(self):
        mode = self.mode
        if 'Years' in mode or 'Months' in mode or 'Days' in mode:
            return True
        else:
            return False

    @autopilot_logging.log_action(logger.info)
    def _pick_year(self, year):
        picker = self.select_single(
            'Picker', objectName='PickerRow_YearPicker')
        list_view = picker.select_single(
            _qquicklistview.QQuickListView, objectName='Picker_Linear')
        self._pick_value(self.year, year, list_view)

    @autopilot_logging.log_action(logger.info)
    def _pick_month(self, month):
        self._pick_value_from_path_view('Month', self.month, month)

    @autopilot_logging.log_action(logger.info)
    def _pick_day(self, day):
        # Python's date object starts at one. The model in the date picker
        # at 0.
        self._pick_value_from_path_view('Day', self.day - 1, day - 1)

    def _pick_value_from_path_view(self, type_, current_value, new_value):
        picker = self.select_single(
            'Picker', objectName='PickerRow_{}Picker'.format(type_))
        path_view = picker.select_single(
            QQuickPathView, objectName='Picker_WrapAround')
        self._pick_value(current_value, new_value, path_view)

    def _pick_value(self, current_value, new_value, scrollable):
        if new_value > current_value:
            direction = 'below'
        elif new_value < current_value:
            direction = 'above'
        else:
            logger.debug('The value is already selected.')
            return
        scrollable.click_element(
            object_name='PickerRow_PickerLabel{}'.format(new_value),
            direction=direction)

    def get_date(self):
        """Return the currently selected date.

        :return: a python datetime.date object with the selected date.

        """
        # Python's date object starts at one. The model in the date picker
        # at 0.
        return datetime.date(self.year, self.month + 1, self.day)

    @autopilot_logging.log_action(logger.info)
    def pick_time(self, time):
        """Pick a time from the date picker.

        :parameter time: The time to pick.
        :type time: An object with hour, minute and second attributes, like
            python's datetime.time.
        :raises ubuntuuitoolkit.ToolkitException if the mode of the picker
            doesn't let select a time.

        """
        if not self._is_time_picker():
            raise _common.ToolkitException(
                "Can't pick time. The picker mode is: {!r}.".format(self.mode))
        # Workaround https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1346669
        # By setting seconds, then minutes, then hours, erratic behavoir
        # can be dealt with
        if 'Seconds' in self.mode:
            self._pick_second(time.second)
            self.seconds.wait_for(time.second)
        if 'Minutes' in self.mode:
            self._pick_minute(time.minute)
            self.minutes.wait_for(time.minute)
        if 'Hours' in self.mode:
            self._pick_hour(time.hour)
            self.hours.wait_for(time.hour)

    def _is_time_picker(self):
        mode = self.mode
        if 'Hours' in mode or 'Minutes' in mode or 'Seconds' in mode:
            return True
        else:
            return False

    @autopilot_logging.log_action(logger.info)
    def _pick_hour(self, hour):
        self._pick_value_from_path_view('Hours', self.hours, hour)

    @autopilot_logging.log_action(logger.info)
    def _pick_minute(self, minute):
        self._pick_value_from_path_view('Minutes', self.minutes, minute)

    @autopilot_logging.log_action(logger.info)
    def _pick_second(self, second):
        self._pick_value_from_path_view('Seconds', self.seconds, second)

    def get_time(self):
        """Return the currently selected time.

        :return: a python datetime.time object with the selected time.

        """
        return datetime.time(self.hours, self.minutes, self.seconds)

# Containers helpers.


def _get_visible_container_top(containers):
    containers_top = [container.globalRect.y for container in containers]
    return max(containers_top)


def _get_visible_container_bottom(containers):
    containers_bottom = [
        container.globalRect.y + container.globalRect.height
        for container in containers if container.globalRect.height > 0]
    return min(containers_bottom)


class Scrollable(_common.UbuntuUIToolkitCustomProxyObjectBase):

    @autopilot_logging.log_action(logger.info)
    def is_child_visible(self, child):
        """Determine if the child is visible.

        A child is visible if no scrolling is needed to reveal it.

        """
        containers = self._get_containers()
        return self._is_child_visible(child, containers)

    def _get_containers(self):
        """Return a list with the containers to take into account when swiping.

        The list includes this flickable and the top-most container.
        TODO add additional flickables that are between this and the top
        container. --elopio - 2014-03-22

        """
        containers = [self._get_top_container(), self]
        return containers

    def _is_child_visible(self, child, containers):
        """Check if the center of the child is visible.

        :return: True if the center of the child is visible, False otherwise.

        """
        object_center = child.globalRect.y + child.globalRect.height // 2
        visible_top = _get_visible_container_top(containers)
        visible_bottom = _get_visible_container_bottom(containers)
        return (object_center >= visible_top and
                object_center <= visible_bottom)

    def _slow_drag_rate(self):
        # I found that when the flickDeceleration is 1500, the rate should be
        # 5 and that when it's 100, the rate should be 1. With those two points
        # we can get the following equation.
        # XXX The deceleration might not be linear with respect to the rate,
        # but this works for the two types of scrollables we have for now.
        # --elopio - 2014-05-08
        return (self.flickDeceleration + 250) / 350

    def _slow_drag(self, start_x, stop_x, start_y, stop_y, rate=None):
        # If we drag too fast, we end up scrolling more than what we
        # should, sometimes missing the  element we are looking for.

        # FIXME: QQuickPathView has no contentY property, but it was added
        # to the PathView used inside the Picker in Picker.qml
        original_content_y = self.contentY

        if rate is None:
            rate = self._slow_drag_rate()
        self.pointing_device.drag(start_x, start_y, stop_x, stop_y, rate=rate)

        if self.contentY == original_content_y:
            raise _common.ToolkitException('Could not swipe in the flickable.')


class QQuickPathView(Scrollable):

    # TODO make it more general and move it to its own module.
    # --elopio - 2014-05-06

    @autopilot_logging.log_action(logger.info)
    def click_element(self, object_name, direction='below'):
        try:
            element = self.select_single(objectName=object_name)
        except dbus.StateNotFoundError:
            # The element might be on a part of the list that hasn't been
            # created yet. We have to search for it scrolling.
            element = self._find_element(object_name, direction)
        self.swipe_child_into_view(element)
        self.pointing_device.click_object(element)

    @autopilot_logging.log_action(logger.info)
    def _find_element(self, object_name, direction):
        containers = self._get_containers()
        for index in range(self.count):
            if direction == 'below':
                swipe_method = self._swipe_to_show_one_more_below
            elif direction == 'above':
                swipe_method = self._swipe_to_show_one_more_above
            else:
                raise _common.ToolkitException(
                    'Invalid direction: {}'.format(direction))

            swipe_method(containers)

            try:
                return self.select_single(objectName=object_name)
            except dbus.StateNotFoundError:
                pass
        raise _common.ToolkitException(
            'List element with objectName "{}" not found.'.format(object_name))

    @autopilot_logging.log_action(logger.info)
    def _swipe_to_show_one_more_above(self, containers):
        self._swipe_to_show_one_more('above', containers)

    @autopilot_logging.log_action(logger.info)
    def _swipe_to_show_one_more_below(self, containers):
        self._swipe_to_show_one_more('below', containers)

    def _swipe_to_show_one_more(self, direction, containers):
        start_x = stop_x = self.globalRect.x + (self.globalRect.width // 2)
        center_y = self.globalRect.y + (self.globalRect.height // 2)
        # XXX This assumes all the children are of the same height
        child = self.get_children_by_type('PickerDelegate')[0]
        top = center_y - (child.globalRect.height // 2)
        bottom = center_y + (child.globalRect.height // 2)
        if direction == 'below':
            start_y = bottom
            stop_y = top
        elif direction == 'above':
            start_y = top
            stop_y = bottom
        else:
            raise _common.ToolkitException(
                'Invalid direction {}.'.format(direction))
        self._slow_drag(start_x, stop_x, start_y, stop_y)
        self.dragging.wait_for(False)
        self.moving.wait_for(False)

    @autopilot_logging.log_action(logger.info)
    def swipe_child_into_view(self, child):
        """Make the child visible.

        Currently it works only when the object needs to be swiped vertically.
        TODO implement horizontal swiping. --elopio - 2014-03-21

        """
        containers = self._get_containers()
        if not self._is_child_visible(child, containers):
            self._swipe_non_visible_child_into_view(child, containers)
        else:
            logger.debug('The element is already visible.')

    @autopilot_logging.log_action(logger.info)
    def _swipe_non_visible_child_into_view(self, child, containers):
        while not self._is_child_visible(child, containers):
            # Check the direction of the swipe based on the position of the
            # child relative to the immediate flickable container.
            if child.globalRect.y < self.globalRect.y:
                self._swipe_to_show_one_more_above(containers)
            else:
                self._swipe_to_show_one_more_below(containers)
