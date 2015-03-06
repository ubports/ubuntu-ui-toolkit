/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.2
import QtQuick.Layouts 1.1

Item {
    width: units.gu(40)
    height: units.gu(71)

    Column {
        width: parent.width
        ListItem {
            RowLayout {
                anchors.fill: parent
                Captions {
                    id: defaults
                }
            }
        }
        ListItem {
            RowLayout {
                anchors.fill: parent
                Captions {
                    id: testCaptions
                }
            }
        }
    }

    UbuntuTestCase {
        name: "CaptionsAPI"
        when: windowShown

        function test_0_defaults() {
            compare(defaults.captionStyle, Ubuntu.TitleCaptionStyle, "Default preset is 'titles'");
            compare(defaults.height, 0, "default height is 0");
            compare(defaults.width, 0, "default width is 0")
            compare(defaults.spacing, units.gu(0.5), "default spacing failure");
            compare(defaults.visible, false, "default should be invisible")
            compare(defaults.Layout.alignment, Qt.AlignVCenter | Qt.AlignLeft, "center vertically and left horizontally by default");
        }

        function test_captions_data() {
            return [
                {tag: "caption, title.fontSize", captionStyle: Ubuntu.TitleCaptionStyle, label: "title", property: "fontSize", value: "medium"},
                {tag: "caption, title.horizontalAlignment", captionStyle: Ubuntu.TitleCaptionStyle, label: "title", property: "horizontalAlignment", value: Text.AlignLeft},
                {tag: "caption, title.elide", captionStyle: Ubuntu.TitleCaptionStyle, label: "title", property: "elide", value: Text.ElideRight},
                {tag: "caption, subtitle.fontSize", captionStyle: Ubuntu.TitleCaptionStyle, label: "subtitle", property: "fontSize", value: "small"},
                {tag: "caption, subtitle.horizontalAlignment", captionStyle: Ubuntu.TitleCaptionStyle, label: "subtitle", property: "horizontalAlignment", value: Text.AlignLeft},
                {tag: "caption, subtitle.maximumLineCount", captionStyle: Ubuntu.TitleCaptionStyle, label: "subtitle", property: "maximumLineCount", value: 2},
                {tag: "caption, subtitle.wrapMode", captionStyle: Ubuntu.TitleCaptionStyle, label: "subtitle", property: "wrapMode", value: Text.Wrap},
                {tag: "caption, subtitle.elide", captionStyle: Ubuntu.TitleCaptionStyle, label: "subtitle", property: "elide", value: Text.ElideRight},

                {tag: "summary, title.fontSize", captionStyle: Ubuntu.SummaryCaptionStyle, label: "title", property: "fontSize", value: "small"},
                {tag: "summary, title.horizontalAlignment", captionStyle: Ubuntu.SummaryCaptionStyle, label: "title", property: "horizontalAlignment", value: Text.AlignRight},
                {tag: "summary, title.elide", captionStyle: Ubuntu.SummaryCaptionStyle, label: "title", property: "elide", value: Text.ElideNone},
                {tag: "summary, subtitle.fontSize", captionStyle: Ubuntu.SummaryCaptionStyle, label: "subtitle", property: "fontSize", value: "small"},
                {tag: "summary, subtitle.horizontalAlignment", captionStyle: Ubuntu.SummaryCaptionStyle, label: "subtitle", property: "horizontalAlignment", value: Text.AlignRight},
                {tag: "summary, subtitle.maximumLineCount", captionStyle: Ubuntu.SummaryCaptionStyle, label: "subtitle", property: "maximumLineCount", value: 1},
                {tag: "summary, subtitle.wrapMode", captionStyle: Ubuntu.SummaryCaptionStyle, label: "subtitle", property: "wrapMode", value: Text.NoWrap},
                {tag: "summary, subtitle.elide", captionStyle: Ubuntu.SummaryCaptionStyle, label: "subtitle", property: "elide", value: Text.ElideNone},
            ];
        }
        function test_captions(data) {
            testCaptions.captionStyle = data.captionStyle;
            compare(testCaptions[data.label][data.property], data.value, data.tag + " values differ");
        }
    }
}
