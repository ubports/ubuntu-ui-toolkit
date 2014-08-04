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

import os

import testtools

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class QQuickGridViewClassTestCase(testtools.TestCase):

    def test_qquickgridview_custom_proxy_object_inherits_from_flickable(self):
        self.assertTrue(
            issubclass(
                ubuntuuitoolkit.QQuickGridView,
                ubuntuuitoolkit.QQuickFlickable))


class QQuickGridViewTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_qquickgridview.QQuickGridViewTestCase.qml')

    def test_select_qquickgridview_must_return_custom_proxy_object(self):
        grid_view = self.main_view.select_single(
            ubuntuuitoolkit.QQuickGridView, objectName='testListView')
        self.assertIsInstance(grid_view, ubuntuuitoolkit.QQuickGridView)
