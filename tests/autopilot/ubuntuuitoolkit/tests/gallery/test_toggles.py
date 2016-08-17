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

import testscenarios

from testtools.matchers import Equals

from ubuntuuitoolkit import emulators, ubuntu_scenarios
from ubuntuuitoolkit.tests import gallery


class EnabledTogglesTestCase(gallery.GalleryTestCase):

    toggles_scenarios = [
        ('checkbox unchecked', dict(
            object_name='checkbox_unchecked', initial_state=False)),
        ('checkbox checked', dict(
            object_name='checkbox_checked', initial_state=True)),
        ('switch unchecked', dict(
            object_name='switch_unchecked', initial_state=False)),
        ('switch checked', dict(
            object_name='switch_checked', initial_state=True))
    ]

    scenarios = testscenarios.multiply_scenarios(
        ubuntu_scenarios.get_device_simulation_scenarios(),
        toggles_scenarios)

    def setUp(self):
        super().setUp()
        self.open_page('togglesElement')

    def test_change_toggles_state(self):
        toggle = self.main_view.select_single(
            emulators.CheckBox, objectName=self.object_name)
        self.assertThat(toggle.enabled, Equals(True))
        self.assertThat(toggle.checked, Equals(self.initial_state))

        toggle.change_state(timeout=1)
        self.assertThat(toggle.checked, Equals(not self.initial_state))


class DisabledTogglesTestCase(gallery.GalleryTestCase):

    toggles_scenarios = [
        ('checkbox disabled unchecked', dict(
            object_name='checkbox_disabled_unchecked', initial_state=False)),
        ('checkbox disabled checked', dict(
            object_name='checkbox_disabled_checked', initial_state=True)),
        ('switch disabled unchecked', dict(
            object_name='switch_disabled_unchecked', initial_state=False)),
        ('switch disabled checked', dict(
            object_name='switch_disabled_checked', initial_state=True))
    ]

    scenarios = testscenarios.multiply_scenarios(
        ubuntu_scenarios.get_device_simulation_scenarios(),
        toggles_scenarios)

    def setUp(self):
        super().setUp()
        self.open_page('togglesElement')

    def test_change_toggles_state(self):
        toggle = self.main_view.select_single(
            emulators.CheckBox, objectName=self.object_name)
        self.assertThat(toggle.enabled, Equals(False))
        self.assertThat(toggle.checked, Equals(self.initial_state))

        self.assertRaises(AssertionError, toggle.change_state, timeout=1)
