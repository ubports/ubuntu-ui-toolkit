# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013-2015 Canonical Ltd.
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

from unittest import mock

import logging
import os

import fixtures
import testtools
from testtools.matchers import Contains

import ubuntuuitoolkit
from ubuntuuitoolkit import listitems, tests


class ListItemsTestCase(testtools.TestCase):

    def test_supported_class(self):
        self.assertTrue(issubclass(
            listitems.Base, listitems.Empty))
        self.assertTrue(issubclass(
            listitems.Expandable, listitems.Empty))
        self.assertTrue(issubclass(
            listitems.ItemSelector, listitems.Empty))
        self.assertTrue(issubclass(
            listitems.Standard, listitems.Empty))
        self.assertTrue(issubclass(
            listitems.SingleControl, listitems.Empty))
        self.assertTrue(issubclass(
            listitems.MultiValue, listitems.Base))
        self.assertTrue(issubclass(
            listitems.SingleValue, listitems.Base))
        self.assertTrue(issubclass(
            listitems.Subtitled, listitems.Base))


class SwipeToDeleteTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_listitems.SwipeToDeleteTestCase.qml')

    def setUp(self):
        super().setUp()
        self._item = self.main_view.select_single(
            listitems.Standard, objectName='listitem_standard')
        self.assertTrue(self._item.exists())

    def test_standard_custom_proxy_object(self):
        self.assertIsInstance(self._item, listitems.Standard)

    def test_swipe_item(self):
        self._item.swipe_to_delete()
        self.assertTrue(self._item.waitingConfirmationForRemoval)

    def test_swipe_item_to_right(self):
        self._item.swipe_to_delete('right')
        self.assertTrue(self._item.waitingConfirmationForRemoval)

    def test_swipe_item_to_left_must_raise_exception(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            self._item.swipe_to_delete,
            'left')
        self.assertEqual(
            str(error),
            'Only swiping to the right will cause the item to be deleted.')

    def test_swipe_item_with_direction_must_log_deprecation_warning(self):
        fake_logger = fixtures.FakeLogger(level=logging.WARNING)
        self.useFixture(fake_logger)

        self._item.swipe_to_delete(direction='right')
        self.assertThat(
            fake_logger.output,
            Contains(
                'The direction argument is deprecated. Now the Ubuntu SDK '
                'only deletes list items when swiping from left to right. '
                'Call swipe_to_delete without arguments.'))

    def test_swipe_item_to_wrong_direction(self):
        self.assertRaises(
            ubuntuuitoolkit.ToolkitException,
            self._item.swipe_to_delete, 'up')

    def test_delete_item_moving_right(self):
        self._item.swipe_to_delete('right')
        self._item.confirm_removal()
        self.assertFalse(self._item.exists())

    def test_delete_non_removable_item(self):
        self._item = self.main_view.select_single(
            listitems.Empty, objectName='listitem_empty')
        self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self._item.swipe_to_delete)

    def test_confirm_removal_when_item_was_not_swiped(self):
        self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self._item.confirm_removal)

    def test_delete_item_without_confirm(self):
        item = self.main_view.select_single(
            listitems.Standard, objectName='listitem_without_confirm')
        item.swipe_to_delete()
        self.assertFalse(item.exists())

    def test_delete_item_with_confirmation_that_will_be_destroyed(self):
        item = self.main_view.select_single(
            listitems.Standard,
            objectName='listitem_destroyed_on_remove_with_confirm')
        item.swipe_to_delete()
        item.confirm_removal()
        self.assertFalse(item.exists())

    def test_delete_item_without_confirmation_that_will_be_destroyed(self):
        item = self.main_view.select_single(
            listitems.Standard,
            objectName='listitem_destroyed_on_remove_without_confirm')
        item.swipe_to_delete()
        self.assertFalse(item.exists())


class ExpandableTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_listitems.ExpandableTestCase.qml')

    def setUp(self):
        super().setUp()
        self.test_expandable = self.main_view.select_single(
            listitems.Expandable, objectName='expandable0')

    def test_expand_item(self):
        self.test_expandable.expand()
        self.assertTrue(self.test_expandable.expanded)

    def test_expand_already_expanded_item_must_do_nothing(self):
        self.test_expandable.expand()

        # Replace the pointer with None, so we make sure it's not being called.
        with mock.patch.object(self.test_expandable, 'pointing_device', None):
            self.test_expandable.expand()

    def test_collapse_item(self):
        self.test_expandable.expand()

        self.test_expandable.collapse()
        self.assertFalse(self.test_expandable.expanded)

    def test_collapse_already_collapsed_item_must_do_nothing(self):
        self.test_expandable.collapse()

        # Replace the pointer with None, so we make sure it's not being called.
        with mock.patch.object(self.test_expandable, 'pointing_device', None):
            self.test_expandable.collapse()
