# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013, 2014 Canonical Ltd.
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

try:
    from unittest import mock
except ImportError:
    import mock

import logging
import os

import fixtures
import testtools
from testtools.matchers import Contains

import ubuntuuitoolkit
from ubuntuuitoolkit import listitems, tests


class ListItemTestCase(tests.QMLFileAppTestCase):
    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_listitems.ListItemTestCase.qml')

    def setUp(self):
        super(ListItemTestCase, self).setUp()
        self.test_listitem = self.main_view.select_single(
            'UCListItem', objectName='listitem0')
        self.test_label = self.main_view.select_single(
            'Label', objectName='triggeredAction')
        self.assertEqual(self.test_label.text, '')

    def test_trigger_delete(self):
        self.test_listitem.click_leading_action('delete_action')
        # check whether action is reported in the triggeredAction
        self.assertEqual(self.test_label.text, 'delete_action')

    def test_trigger_search(self):
        self.test_listitem.click_trailing_action('search_action')
        # check whether action is reported in the triggeredAction
        self.assertEqual(self.test_label.text, 'search_action')

    def test_trigger_edit(self):
        self.test_listitem.click_trailing_action('edit_action')
        # check whether action is reported in the triggeredAction
        self.assertEqual(self.test_label.text, 'edit_action')

    def test_trigger_email(self):
        self.test_listitem.click_trailing_action('email_action')
        # check whether action is reported in the triggeredAction
        self.assertEqual(self.test_label.text, 'email_action')
