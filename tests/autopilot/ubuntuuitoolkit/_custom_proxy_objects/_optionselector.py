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


import time

from autopilot.introspection import dbus

from ubuntuuitoolkit._custom_proxy_objects import _common


class OptionSelector(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """OptionSelector Autopilot custom proxy object"""

    def get_option_count(self):
        """Gets the number of items in the option selector"""
        self.list_view = self.select_single("QQuickListView")
        return self.list_view.count

    def get_selected_index(self):
        """Gets the current selected index of the QQuickListView"""
        self.list_view = self.select_single("QQuickListView")
        return self.list_view.currentIndex

    def get_selected_text(self):
        """gets the text of the currently selected item"""
        option_selector_delegate = self.select_single(
            'OptionSelectorDelegate', focus='True')
        current_label = option_selector_delegate.select_single(
            'UCLabel', visible='True')
        return current_label.text

    def get_current_label(self):
        """gets the text of the currently selected item"""
        option_selector_delegate = self.select_single(
            'OptionSelectorDelegate', focus='True')
        current_label = option_selector_delegate.select_single(
            'UCLabel', visible='True')
        return current_label

    def _expand(self):
        """Expand an optionselector if it's collapsed"""
        # if just collapsed it can think that the item is expanded
        #  https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1240288
        time.sleep(1)
        if not self.expanded and not self.currentlyExpanded:
            self.pointing_device.click_object(self.get_current_label())
            self.currentlyExpanded.wait_for(True)
            # selecting the same item too quickly after expand
            # causes the wrong item to be selected
            # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1231939
            time.sleep(1)

    def select_option(self, *args, **kwargs):
        """Select delegate in option selector

        Example usage::
            select_option(objectName="myOptionSelectorDelegate")
            select_option('UCLabel', text="some_text_here")

        :parameter kwargs: keywords used to find property(s) of delegate in
            option selector

        """

        if args:
            try:
                select_object = self.select_single(
                    *args,
                    **kwargs
                )
            except dbus.StateNotFoundError:
                raise _common.ToolkitException(
                    'OptionSelectorDelegate with args {} and kwargs {} not '
                    'found'.format(args, kwargs)
                )

        else:
            try:
                select_object = self.select_single(
                    'OptionSelectorDelegate',
                    **kwargs
                )
            except dbus.StateNotFoundError:
                raise _common.ToolkitException(
                    'OptionSelectorDelegate with kwargs {} not found'.format(
                        kwargs)
                )

        self._expand()
        self.pointing_device.click_object(select_object)
