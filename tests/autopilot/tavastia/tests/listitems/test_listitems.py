# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Tests for the ListView Qml component."""

from autopilot.matchers import Eventually
from textwrap import dedent
from testtools.matchers import Is, Not, Equals
from testtools import skip

from tavastia.tests import TavastiaTestCase

class ListViewTests(TavastiaTestCase):
    """Tests for ListView component."""

    test_qml = dedent("""\
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    Column {
        width: 250

        ListModel {
            id: testModel
            ListElement { name: "Parsley" }
            ListElement { name: "Orange"; type: "fruit"}
            ListElement { name: "Apple"; type: "fruit" }
            ListElement { name: "Tomato"; type: "fruit" }

            ListElement { name: "Carrot"; type: "veg" }
            ListElement { name: "Potato"; type: "veg" }
        }
        ListView {
            id: groupedList
            model: testModel
            width: parent.width
            interactive: false

            delegate: ListItem.Standard {
                text: name
            }
            header: ListItem.Header { text: "Grouped List" }
            section.property: "type"
            section.criteria: ViewSection.FullString
            section.delegate: ListItem.Header { text: section }
        }
    }
    """)

    def test_can_select_listview(self):
        """Must be able to select the Qml ListView component."""

        obj = self.app.select_single('ListView')
        self.assertThat(obj, Not(Is(None)))
    

