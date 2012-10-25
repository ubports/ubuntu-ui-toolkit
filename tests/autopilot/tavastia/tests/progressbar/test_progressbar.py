# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the ProgressBar Qml component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip

from tavastia.tests import TavastiaTestCase

class ProgressBarTests(TavastiaTestCase):
    """Tests for ProgressBar component."""

    test_qml = dedent("""\
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    ProgressBar {
       id: progressBar
    }
    """)

    def test_can_select_progressbar(self):
        """Must be able to select the Qml ProgressBar component."""

        obj = self.app.select_single('ProgressBar')
        self.assertThat(obj, Not(Is(None)))
    

