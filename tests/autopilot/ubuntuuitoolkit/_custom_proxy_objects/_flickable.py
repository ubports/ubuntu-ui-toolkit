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

import autopilot.exceptions
from autopilot import logging as autopilot_logging

from ubuntuuitoolkit import units
from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)

# FIXME: Only left-to-right (where x=0 is leftmost) layouts are taken into
#   account. Add support for horizontal flicking with right-to-left layouts.

# Containers helpers.


def _get_visible_container_top(containers):
    containers_top = [container.globalRect.y for container in containers]
    return max(containers_top)


def _get_visible_container_bottom(containers):
    containers_bottom = [
        container.globalRect.y + container.globalRect.height
        for container in containers if container.globalRect.height > 0]
    return min(containers_bottom)


def _get_visible_container_leftmost(containers):
    containers_left = [container.globalRect.x for container in containers]
    return max(containers_left)


def _get_visible_container_rightmost(containers):
    containers_right = [
        container.globalRect.x + container.globalRect.width
        for container in containers if container.globalRect.width > 0]
    return min(containers_right)


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

    def _is_child_in_visible_y_range(self, child, containers):
        """Check if the center of the child has an y-value in the visible range.

        :return: True if the center of the child is y-visible, False otherwise.
        """
        object_center_y = child.globalRect.y + child.globalRect.height // 2
        visible_top = _get_visible_container_top(containers)
        visible_bottom = _get_visible_container_bottom(containers)
        return (object_center_y >= visible_top and
                object_center_y <= visible_bottom)

    def _is_child_in_visible_x_range(self, child, containers):
        """Check if the center of the child has an x-value in the visible range.

        :return: True if the center of the child is x-visible. False otherwise.
        """
        object_center_x = child.globalRect.x + child.globalRect.width // 2
        visible_left = _get_visible_container_leftmost(containers)
        visible_right = _get_visible_container_rightmost(containers)
        return (object_center_x >= visible_left and
                object_center_x <= visible_right)

    def _is_child_visible(self, child, containers):
        """Check if the center of the child is visible.

        :return: True if the center of the child is visible, False otherwise.

        """
        return (self._is_child_in_visible_y_range(child, containers) and
                self._is_child_in_visible_x_range(child, containers))

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

        # FIXME: Not all scrollables have contentX and contentY
        # (PathView does not have them), so use this only in a Flickable.
        original_content_y = self.contentY
        original_content_x = self.contentX

        if rate is None:
            rate = self._slow_drag_rate()
        self.pointing_device.drag(start_x, start_y, stop_x, stop_y, rate=rate)

        if (self.contentY == original_content_y and
           self.contentX == original_content_x):
            raise _common.ToolkitException('Could not swipe in the flickable.')


class QQuickFlickable(Scrollable):

    # Swiping from below can open the deprecated toolbar or trigger the bottom
    # edge gesture. Use this margin to start a swipe that will not be that
    # close to the bottom edge.
    margin_to_swipe_from_bottom = units.gu(2)
    # Swiping from above can open the indicators or resize the window. Use this
    # margin to start a swipe that will not be that close to the top edge.
    margin_to_swipe_from_top = units.gu(1)
    # Swiping from left and right can show the launcher or open app switcher.
    margin_to_swipe_from_left = units.gu(3)
    margin_to_swipe_from_right = units.gu(3)

    @autopilot_logging.log_action(logger.info)
    def swipe_child_into_view(self, child):
        """Make the child visible.

        """
        containers = self._get_containers()
        if not self._is_child_visible(child, containers):
            self._swipe_non_visible_child_into_view(child, containers)
        else:
            logger.debug('The element is already visible.')

    @autopilot_logging.log_action(logger.info)
    def _swipe_non_visible_child_into_view(self, child, containers):
        while not self._is_child_in_visible_y_range(child, containers):
            # Check the direction of the swipe based on the position of the
            # child relative to the immediate flickable container.
            if child.globalRect.y < self.globalRect.y:
                self.swipe_to_show_more_above(containers)
            else:
                self.swipe_to_show_more_below(containers)

        while not self._is_child_in_visible_x_range(child, containers):
            if child.globalRect.x < self.globalRect.x:
                self.swipe_to_show_more_left(containers)
            else:
                self.swipe_to_show_more_right(containers)

    @autopilot_logging.log_action(logger.info)
    def swipe_to_show_more_above(self, containers=None):
        if self.atYBeginning:
            raise _common.ToolkitException(
                "Can't swipe more, we are already at the top of the "
                "container.")
        else:
            self._swipe_to_show_more('above', containers)

    @autopilot_logging.log_action(logger.info)
    def swipe_to_show_more_below(self, containers=None):
        if self.atYEnd:
            raise _common.ToolkitException(
                "Can't swipe more, we are already at the bottom of the "
                "container.")
        else:
            self._swipe_to_show_more('below', containers)

    @autopilot_logging.log_action(logger.info)
    def swipe_to_show_more_left(self, containers=None):
        if self.atXBeginning:
            raise _common.ToolkitException(
                "Can't swipe more, we are already at the left of the "
                "container.")
        else:
            self._swipe_to_show_more('left', containers)

    @autopilot_logging.log_action(logger.info)
    def swipe_to_show_more_right(self, containers=None):
        if self.atXEnd:
            raise _common.ToolkitException(
                "Can't swipe more, we are already at the right of the "
                "container.")
        else:
            self._swipe_to_show_more('right', containers)

    def _swipe_to_show_more(self, direction, containers=None):
        if containers is None:
            containers = self._get_containers()
        start_x = stop_x = self.globalRect.x + (self.globalRect.width // 2)
        start_y = stop_y = self.globalRect.y + (self.globalRect.height // 2)

        top = _get_visible_container_top(containers)
        bottom = _get_visible_container_bottom(containers)
        leftmost = _get_visible_container_leftmost(containers)
        rightmost = _get_visible_container_rightmost(containers)

        # Make the drag range be a multiple of the drag "rate" value.
        # Workarounds https://bugs.launchpad.net/mir/+bug/1399690
        rate = self._slow_drag_rate()

        # The swipes are not done from right at the top and bottom because
        # they could trigger edge gestures or resize windows.
        if direction == 'below':
            start_y = bottom - self.margin_to_swipe_from_bottom
            stop_y = start_y + (top - start_y) / rate * rate

        elif direction == 'above':
            start_y = top + self.margin_to_swipe_from_top
            stop_y = start_y + (bottom - start_y) / rate * rate

        elif direction == 'left':
            start_x = leftmost + self.margin_to_swipe_from_left
            stop_x = start_x + (rightmost - start_x) / rate * rate

        elif direction == 'right':
            start_x = rightmost - self.margin_to_swipe_from_right
            stop_x = start_x + (leftmost - start_x) / rate * rate

        else:
            raise _common.ToolkitException(
                'Invalid direction {}.'.format(direction))
        self._slow_drag(start_x, stop_x, start_y, stop_y, rate)
        self.dragging.wait_for(False)
        self.moving.wait_for(False)

    @autopilot_logging.log_action(logger.info)
    def swipe_to_top(self):
        if not self.atYBeginning:
            containers = self._get_containers()
            while not self.atYBeginning:
                self.swipe_to_show_more_above(containers)

    @autopilot_logging.log_action(logger.info)
    def swipe_to_bottom(self):
        if not self.atYEnd:
            containers = self._get_containers()
            while not self.atYEnd:
                self.swipe_to_show_more_below(containers)

    @autopilot_logging.log_action(logger.info)
    def swipe_to_leftmost(self):
        if not self.atXBeginning:
            containers = self._get_containers()
            while not self.atXBeginning:
                self.swipe_to_show_more_left(containers)

    def swipe_to_rightmost(self):
        if not self.atXEnd:
            containers = self._get_containers()
            while not self.atXEnd:
                self.swipe_to_show_more_right(containers)

    @autopilot_logging.log_action(logger.info)
    def pull_to_refresh(self):
        """Pulls the flickable down and triggers a refresh on it.

        :raises ubuntuuitoolkit.ToolkitException: If the flickable has no pull
            to release functionality.

        """
        try:
            pull_to_refresh = self.select_single(PullToRefresh)
        except autopilot.exceptions.StateNotFoundError:
            raise _common.ToolkitException(
                'The flickable has no pull to refresh functionality.')
        self.swipe_to_top()
        self._swipe_to_middle()
        pull_to_refresh.wait_for_refresh()

    @autopilot_logging.log_action(logger.info)
    def _swipe_to_middle(self):
        start_x = stop_x = self.globalRect.x + (self.globalRect.width // 2)
        # Start just a little under the top.
        containers = self._get_containers()
        top = _get_visible_container_top(containers) + 5
        bottom = _get_visible_container_bottom(containers)

        start_y = top
        stop_y = (self.globalRect.y + bottom) // 2
        self._slow_drag(start_x, stop_x, start_y, stop_y)
        self.dragging.wait_for(False)
        self.moving.wait_for(False)

    @autopilot_logging.log_action(logger.info)
    def _cancel_pull_to_refresh(self):
        """Swipe down the list and then swipe it up to cancel the refresh."""
        # This method is kept private because it's not for the test writers to
        # call. It's to test pull to refresh. --elopio - 2014-05-22
        self.swipe_to_top()
        start_x = stop_x = self.globalRect.x + (self.globalRect.width // 2)
        # Start just a little under the top.
        containers = self._get_containers()
        top = _get_visible_container_top(containers) + 5
        bottom = _get_visible_container_bottom(containers)

        start_y = top
        stop_y = (self.globalRect.y + bottom) // 2
        self.pointing_device.move(start_x, start_y)
        self.pointing_device.press()
        self.pointing_device.move(stop_x, stop_y)
        self.pointing_device.move(start_x, start_y)
        self.pointing_device.release()


class PullToRefresh(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Autopilot helper for the PullToRefresh component."""

    def wait_for_refresh(self):
        activity_indicator = self.select_single('ActivityIndicator')
        activity_indicator.running.wait_for(False)
