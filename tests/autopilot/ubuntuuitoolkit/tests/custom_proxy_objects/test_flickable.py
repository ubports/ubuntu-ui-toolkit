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


import testtools

import ubuntuuitoolkit


class FlickableTestCase(testtools.TestCase):

    def test_get_unity_top_container(self):
        """Test that we can get the top cointainer in Unity."""
        # This tests bug http://pad.lv/1314390
        # On Unity, the top container is not the first child as it is in all
        # the apps that have a MainView. This makes the first implementation of
        # _get_top_container fail. Instead of going from the top looking for
        # a container, we should start from the flickable until we find the
        # top-most container.
        # FIXME we are faking the QML tree because we have no way to launch
        # the app with a tree like the one in Unity8. kalikiana has a branch
        # with an alternate launcher that will let us clean this test.
        # --elopio - 2014-05-15.
        RootClass = type('obj', (object,), {'id': 'root'})
        mock_root_instance = RootClass()
        # We consider a container is an object with a globalRect.
        MockNonContainerClass = type('obj', (object,), {})
        mock_non_container = MockNonContainerClass()
        MockContainerClass = type(
            'obj', (object,), {'id': 'container', 'globalRect': 'dummy'})
        mock_container = MockContainerClass()
        mock_container.get_parent = lambda: mock_root_instance

        # The root instance has two children. This exposes the bug.
        mock_root_instance.get_children = lambda: [
            mock_non_container, mock_container]

        dummy_state = {'id': '10'}
        flickable = ubuntuuitoolkit.Flickable(
            dummy_state, '/dummy'.encode(), 'dummy')

        flickable.get_root_instance = lambda: mock_root_instance
        # The top container of the flickable is its immediate parent.
        flickable.get_parent = lambda: mock_container

        top_container = flickable._get_top_container()
        self.assertEqual(top_container, mock_container)
