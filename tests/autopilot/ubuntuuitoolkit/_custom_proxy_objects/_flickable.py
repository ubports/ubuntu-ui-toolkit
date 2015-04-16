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

import autopilot.exceptions
from autopilot import logging as autopilot_logging

from ubuntuuitoolkit import units
from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)


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
        original_content_y = self.contentY

        if rate is None:
            rate = self._slow_drag_rate()
        self.pointing_device.drag(start_x, start_y, stop_x, stop_y, rate=rate)

        if self.contentY == original_content_y:
            raise _common.ToolkitException('Could not swipe in the flickable.')


class QQuickFlickable(Scrollable):

    # Swiping from below can open the toolbar or trigger the bottom edge
    # gesture. Use this margin to start a swipe that will not be that close to
    # the bottom edge.
    margin_to_swipe_from_bottom = units.gu(2)
    # Swiping from above can open the indicators or resize the window. Use this
    # margin to start a swipe that will not be that close to the top edge.
    margin_to_swipe_from_top = units.gu(1)

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
                self.swipe_to_show_more_above(containers)
            else:
                self.swipe_to_show_more_below(containers)

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

    def _swipe_to_show_more(self, direction, containers=None):
        if containers is None:
            containers = self._get_containers()
        start_x = stop_x = self.globalRect.x + (self.globalRect.width // 2)

        top = _get_visible_container_top(containers)
        bottom = _get_visible_container_bottom(containers)

        # Make the drag range be a multiple of the drag "rate" value.
        # Workarounds https://bugs.launchpad.net/mir/+bug/1399690
        rate = self._slow_drag_rate()

        # The swipes are not done from right at the top and bottom because
        # they could trigger edge gestures or resize windows.
        if direction == 'below':
            start_y = bottom - self.margin_to_swipe_from_bottom
            stop_y = start_y + (top - start_y) // rate * rate

        elif direction == 'above':
            start_y = top + self.margin_to_swipe_from_top
            stop_y = start_y + (bottom - start_y) // rate * rate

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
