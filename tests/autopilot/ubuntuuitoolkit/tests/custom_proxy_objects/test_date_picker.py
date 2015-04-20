# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014, 2015 Canonical Ltd.
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
    objectName: "mainView"

    Column {
        DatePicker {
            id: datePicker
            objectName: 'datePicker'
            mode: 'Years|Months|Days'
            maximum: {
                var d = new Date()
                d.setFullYear('2030')
                return d
            }
            minimum: {
                var d = new Date()
                d.setFullYear('1990')
                return d
            }
            date: {
                var d = new Date()
                d.setFullYear('2010')
                d.setMonth('5')
                d.setDate('15')
                return d
            }
        }
        DatePicker {
            id: timePicker
            objectName: 'timePicker'
            mode: 'Hours|Minutes|Seconds'
            maximum: {
                var d = new Date()
                d.setFullYear('2030')
                return d
            }
            minimum: {
                var d = new Date()
                d.setFullYear('1990')
                return d
            }
            date: {
                var d = new Date()
                d.setHours(12)
                d.setMinutes('30')
                d.setSeconds('30')
                return d
            }
        }
    }
}
""")

    def setUp(self):
        super().setUp()
        self.date_picker = self.main_view.select_single(
            pickers.DatePicker, objectName='datePicker')
        self.time_picker = self.main_view.select_single(
            pickers.DatePicker, objectName='timePicker')


class DatePickerTestCase(DatePickerBaseTestCase):

    def test_select_date_picker_must_return_custom_proxy_object(self):
        self.assertIsInstance(
            self.date_picker, pickers.DatePicker)

    def test_pick_date_on_time_picker_must_raise_exception(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.time_picker.pick_date,
            'dummy')
        self.assertEqual(
            str(error),
            "Can't pick date. The picker mode is: {!r}.".format(
                self.time_picker.mode))

    def test_pick_time_on_date_picker_must_raise_exception(self):
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.date_picker.pick_time,
            'dummy')
        self.assertEqual(
            str(error),
            "Can't pick time. The picker mode is: {!r}.".format(
                self.date_picker.mode))

    def test_swipe_to_show_one_more_below_must_select_next_index(self):
        """Test that we don't end up swiping more than needed.

        This would cause us to miss the element we are looking for, and to have
        to swipe many times in order to finally click it.

        """
        picker = self.main_view.select_single(
            'Picker', objectName='PickerRow_DayPicker')
        path_view = picker.select_single(
            pickers.QQuickPathView, objectName='Picker_WrapAround')
        current_index = path_view.currentIndex

        path_view._swipe_to_show_one_more_below(path_view._get_containers())

        self.assertEqual(path_view.currentIndex, current_index + 1)

    def test_swipe_to_show_one_more_above_must_select_previous_index(self):
        """Test that we don't end up swiping more than needed.

        This would cause us to miss the element we are looking for, and to have
        to swipe many times in order to finally click it.

        """
        picker = self.main_view.select_single(
            'Picker', objectName='PickerRow_DayPicker')
        path_view = picker.select_single(
            pickers.QQuickPathView, objectName='Picker_WrapAround')
        current_index = path_view.currentIndex

        path_view._swipe_to_show_one_more_above(path_view._get_containers())

        self.assertEqual(path_view.currentIndex, current_index - 1)


class PickDateFromDatePickerTestCase(DatePickerBaseTestCase):

    SELECTED_YEAR = 2010
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
        ('change all values', {
            'date_to_pick': datetime.date(
                SELECTED_YEAR - 10, SELECTED_MONTH + 4, SELECTED_DAY - 10)}),
    ]

    def test_pick_date(self):
        """Test that picking a date updates the picker."""
        self.date_picker.pick_date(self.date_to_pick)
        self.assertEqual(self.date_picker.get_date(), self.date_to_pick)


class PickTimeFromDatePickerTestCase(DatePickerBaseTestCase):

    SELECTED_HOUR = 12
    SELECTED_MINUTE = 30
    SELECTED_SECOND = 30

    scenarios = [
        ('higher hour', {
            'time_to_pick': datetime.time(
                SELECTED_HOUR + 6, SELECTED_MINUTE, SELECTED_SECOND)}),
        ('lower hour', {
            'time_to_pick': datetime.time(
                SELECTED_HOUR - 6, SELECTED_MINUTE, SELECTED_SECOND)}),
        ('higher minute', {
            'time_to_pick': datetime.time(
                SELECTED_HOUR, SELECTED_MINUTE + 10, SELECTED_SECOND)}),
        ('lower minute', {
            'time_to_pick': datetime.time(
                SELECTED_HOUR, SELECTED_MINUTE - 10, SELECTED_SECOND)}),
        ('higher second', {
            'time_to_pick': datetime.time(
                SELECTED_HOUR, SELECTED_MINUTE, SELECTED_SECOND + 10)}),
        ('lower second', {
            'time_to_pick': datetime.time(
                SELECTED_HOUR, SELECTED_MINUTE, SELECTED_SECOND - 10)}),
        ('change all values', {
            'time_to_pick': datetime.time(
                SELECTED_HOUR + 6, SELECTED_MINUTE - 10,
                SELECTED_SECOND + 10)}),
    ]

    def test_pick_time(self):
        self.time_picker.pick_time(self.time_to_pick)
        self.assertEqual(self.time_picker.get_time(), self.time_to_pick)
