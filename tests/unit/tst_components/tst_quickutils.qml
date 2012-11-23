/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1


TestCase {
    name: "QuickUtilsAPI"

    function test_modelDelegateHeightForObjectModel()
    {
        list.model = objectList;
        list.section.property = "label";

        var sectionHeight = QuickUtils.modelDelegateHeight(list.section.delegate, list.model);
        compare(sectionHeight, 25, "sectionHeight failure");

        var itemHeight = QuickUtils.modelDelegateHeight(list.delegate, list.model);
        compare(itemHeight, 50, "itemHeight failure");
    }

    function test_modelDelegateHeightForVariantListModel()
    {
        list.model = variantList;
        list.section.property = "label";

        var sectionHeight = QuickUtils.modelDelegateHeight(list.section.delegate, list.model);
        compare(sectionHeight, 25, "sectionHeight failure");

        var itemHeight = QuickUtils.modelDelegateHeight(list.delegate, list.model);
        compare(itemHeight, 50, "itemHeight failure");
    }

    function test_modelDelegateHeightForStringListModel()
    {
        list.model = stringList;
        list.section.property = "modelData";

        var sectionHeight = QuickUtils.modelDelegateHeight(list.section.delegate, list.model);
        compare(sectionHeight, 25, "sectionHeight failure");

        var itemHeight = QuickUtils.modelDelegateHeight(list.delegate, list.model);
        compare(itemHeight, 50, "itemHeight failure");
    }

    function test_modelDelegateHeightForNumericModel()
    {
        list.model = 100;
        list.section.property = "modelData";

        var sectionHeight = QuickUtils.modelDelegateHeight(list.section.delegate, list.model);
        compare(sectionHeight, 25, "sectionHeight failure");

        var itemHeight = QuickUtils.modelDelegateHeight(list.delegate, list.model);
        compare(itemHeight, 50, "itemHeight failure");
    }

    property var objectList: ListModel {
        ListElement {label: "1"}
        ListElement {label: "10"}
        ListElement {label: "100"}
        ListElement {label: "2"}
        ListElement {label: "3"}
        ListElement {label: "4"}
        ListElement {label: "5"}
        ListElement {label: "6"}
        ListElement {label: "7"}
        ListElement {label: "8"}
    }

    property var variantList: [
        {"label": "1"},
        {"label": "10"},
        {"label": "100"},
        {"label": "2"},
        {"label": "3"},
        {"label": "4"},
        {"label": "5"},
        {"label": "6"},
        {"label": "7"},
        {"label": "8"}
    ]

    property var stringList: ["1", "10", "100", "2", "3", "4", "5", "6", "7", "8", "9"]


    ListView {
        id: list

        section.criteria: ViewSection.FirstCharacter
        section.delegate: Header{
            text: section
            height: 25
        }

        delegate: Standard {
            text: label
            height: 50
        }
    }

}
