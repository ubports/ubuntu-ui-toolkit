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
import fixtures
import logging
from testtools.matchers import Contains


class TestUbuntuUIToolkitImportTestCase(testtools.TestCase):

    def test_import_toolkit_must_not_log_warning(self):
        fake_logger = fixtures.FakeLogger(level=logging.WARNING)
        self.useFixture(fake_logger)
        import ubuntuuitoolkit
        self.assertFalse(
            fake_logger.output,
            Contains(
                'The ubuntuuitoolkit.emulators module is deprecated. Import '
                'the autopilot helpers from the top-level ubuntuuitoolkit '
                'module.'))

        class BlankClass(ubuntuuitoolkit.base.UbuntuUIToolkitAppTestCase):
            """Empty class to avoid pyflakes error"""
