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

import datetime

import ubuntuuitoolkit
from ubuntuuitoolkit import pickers, tests


class DatePickerBaseTestCase(tests.QMLStringAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Pickers 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Column {
        DatePicker {
            id: datePicker
            objectName: 'datePicker'
            mode: 'Years|Months|Days'
            date: {
                var d = new Date()
                // Make sure that the picker will have higher and lower values
                // to select.
                d.setFullYear(d.getFullYear() + 25)
                d.setMonth('5')
                d.setDate('15')
                return d
            }
        }
        DatePicker {
            id: timePicker
            objectName: 'timePicker'
            mode: 'Hours|Minutes|Seconds'
        }
    }
}
""")

    def setUp(self):
        super(DatePickerBaseTestCase, self).setUp()
        self.date_picker = self.main_view.select_single(
            pickers.DatePicker, objectName='datePicker')


class DatePickerTestCase(DatePickerBaseTestCase):

    def test_select_date_picker_must_return_custom_proxy_object(self):
        self.assertIsInstance(
            self.date_picker, pickers.DatePicker)

    def test_pick_date_on_time_picker_must_raise_exception(self):
        time_picker = self.main_view.select_single(
            pickers.DatePicker, objectName='timePicker')
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, time_picker.pick_date, 'dummy')
        self.assertEqual(
            str(error),
            "Can't pick date. The picker mode is: {!r}.".format(
                time_picker.mode))

    def test_pick_value_in_next_page_swipes_only_once(self):
        """Test that we don't end up swiping more than needed.

        This would cause us to miss the element we are looking for, and to have
        to swipe many times in order to finally click it.

        """
        # TODO ask mfoord. DO NOT MERGE YET
        pass


class PickDateFromDatePickerTestCase(DatePickerBaseTestCase):

    SELECTED_YEAR = datetime.date.today().year + 25
    SELECTED_MONTH = 6  # June
    SELECTED_DAY = 15

    scenarios = [
        ('higher year', {
            'date_to_pick': datetime.date(
                SELECTED_YEAR + 10, SELECTED_MONTH, SELECTED_DAY)}),
        ('lower year', {
            'date_to_pick': datetime.date(
                SELECTED_YEAR - 10, SELECTED_MONTH, SELECTED_DAY)}),
        ('higher month', {
            'date_to_pick': datetime.date(
                SELECTED_YEAR, SELECTED_MONTH + 4, SELECTED_DAY)}),
        ('lower month', {
            'date_to_pick': datetime.date(
                SELECTED_YEAR, SELECTED_MONTH - 4, SELECTED_DAY)}),
        ('higher day', {
            'date_to_pick': datetime.date(
                SELECTED_YEAR, SELECTED_MONTH, SELECTED_DAY + 10)}),
        ('lower day', {
            'date_to_pick': datetime.date(
                SELECTED_YEAR, SELECTED_MONTH, SELECTED_DAY - 10)}),
        ('change all value', {
            'date_to_pick': datetime.date(
                SELECTED_YEAR - 10, SELECTED_MONTH + 4, SELECTED_DAY - 10)}),
    ]

    def test_pick_date(self):
        self.date_picker.pick_date(self.date_to_pick)
        self.assertEqual(self.date_picker.get_date(), self.date_to_pick)
