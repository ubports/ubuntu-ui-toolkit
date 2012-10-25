# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the QML Slider component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip

from tavastia.tests import TavastiaTestCase

class SliderTests(TavastiaTestCase):
    """Tests for Slider component."""

    test_qml = dedent("""\
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    Slider {
       id: slider
    }
    """)

    def test_can_select_slider(self):
        """Must be able to select the Qml Slider component."""

        obj = self.app.select_single('Slider')
        self.assertThat(obj, Not(Is(None)))
    

