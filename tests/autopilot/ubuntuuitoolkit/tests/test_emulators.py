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

import imp
import logging

import fixtures
import testscenarios
import testtools
from testtools.matchers import Contains

import ubuntuuitoolkit
from ubuntuuitoolkit import emulators, listitems, popups


class DeprecatedSymbolsTestCase(testscenarios.TestWithScenarios):

    symbols_retaining_name = [
        'check_autopilot_version', 'get_keyboard', 'get_pointing_device',
        'CheckBox', 'Header', 'MainView', 'OptionSelector', 'QQuickFlickable',
        'QQuickListView', 'TabBar', 'Tabs', 'TextField', 'Toolbar',
    ]

    symbols_retaining_name_scenarios = [
        (symbol, dict(
            current_module=ubuntuuitoolkit, current_symbol=symbol,
            deprecated_symbol=symbol))
        for symbol in symbols_retaining_name
    ]

    symbols_with_new_name = [
        (ubuntuuitoolkit, 'ToolkitException', 'ToolkitEmulatorException'),
        (ubuntuuitoolkit, 'UbuntuUIToolkitCustomProxyObjectBase',
         'UbuntuUIToolkitEmulatorBase'),
        (popups, 'ActionSelectionPopover', 'ActionSelectionPopover'),
        (popups, 'ComposerSheet', 'ComposerSheet'),
        (listitems, 'Base', 'Base'),
        (listitems, 'Empty', 'Empty'),
        (listitems, 'ItemSelector', 'ItemSelector'),
        (listitems, 'MultiValue', 'MultiValue'),
        (listitems, 'SingleControl', 'SingleControl'),
        (listitems, 'SingleValue', 'SingleValue'),
        (listitems, 'Standard', 'Standard'),
        (listitems, 'Subtitled', 'Subtitled')
    ]

    symbols_with_new_name_scenarios = [
        ('{0} to {1}'.format(old_name, new_name),
         dict(
             current_module=new_module, current_symbol=new_name,
             deprecated_symbol=old_name))
        for new_module, new_name, old_name in symbols_with_new_name]

    scenarios = (symbols_retaining_name_scenarios +
                 symbols_with_new_name_scenarios)

    def test_deprecated_symbol_must_be_the_same_as_current(self):
        self.assertEqual(
            getattr(self.current_module, self.current_symbol),
            getattr(emulators, self.deprecated_symbol))


class DeprecationWarningTestCase(testtools.TestCase):

    def reload_emulators(self):
        _, ubuntuuitoolkit_path, _ = imp.find_module('ubuntuuitoolkit')
        emulators_file, emulators_path, emulators_description = (
            imp.find_module('emulators', [ubuntuuitoolkit_path]))
        imp.load_module(
            'emulators', emulators_file, emulators_path,
            emulators_description)

    def test_import_emulators_must_log_warning(self):
        fake_logger = fixtures.FakeLogger(level=logging.WARNING)
        self.useFixture(fake_logger)
        self.reload_emulators()
        self.assertThat(
            fake_logger.output,
            Contains(
                'The ubuntuuitoolkit.emulators module is deprecated. Import '
                'the autopilot helpers from the top-level ubuntuuitoolkit '
                'module.'))
