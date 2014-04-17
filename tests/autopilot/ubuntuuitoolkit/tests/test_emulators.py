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

"""Tests for the deprecated ubuntuuitoolkit.emulators module."""

import logging

import fixtures
import testscenarios
import testtools
from testtools.matchers import Contains

import ubuntuuitoolkit
from ubuntuuitoolkit import emulators


class DeprecatedSymbolsTestCase(testscenarios.TestWithScenarios):

    symbols_retaining_name = [
        'check_autopilot_version', 'get_keyboard', 'get_pointing_device',
        'ActionSelectionPopover', 'Base', 'CheckBox', 'ComposerSheet',
        'Empty', 'Flickable', 'Header', 'ItemSelector', 'MainView',
        'MultiValue', 'OptionSelector', 'QQuickListView', 'SingleControl',
        'SingleValue', 'Standard', 'Subtitled', 'TabBar', 'Tabs',
        'TextField', 'Toolbar',
    ]

    symbols_retaining_name_scenarios = [
        (symbol, dict(current=symbol, deprecated=symbol))
        for symbol in symbols_retaining_name
    ]

    symbols_with_new_name = [
        ('ToolkitException', 'ToolkitEmulatorException'),
        ('UbuntuUIToolkitCustomProxyObjectBase',
         'UbuntuUIToolkitEmulatorBase')]

    symbols_with_new_name_scenarios = [
        ('{0} to {1}'.format(old_name, new_name),
         dict(current=new_name, deprecated=old_name))
        for new_name, old_name in symbols_with_new_name]

    scenarios = (symbols_retaining_name_scenarios +
                 symbols_with_new_name_scenarios)

    def test_deprecated_symbol_must_be_the_same_as_current(self):
        self.assertEqual(
            getattr(ubuntuuitoolkit, self.current),
            getattr(emulators, self.deprecated))


class DeprecationWarningTestCase(testtools.TestCase):

    def test_import_emulators_must_log_warning(self):
        fake_logger = fixtures.FakeLogger(level=logging.WARNING)
        self.useFixture(fake_logger)
        reload(emulators)
        self.assertThat(
            fake_logger.output,
            Contains(
                'The ubuntuuitoolkit.emulators module is deprecated. Import '
                'the autopilot helpers from the top-level ubuntuuitoolkit '
                'module.'))
