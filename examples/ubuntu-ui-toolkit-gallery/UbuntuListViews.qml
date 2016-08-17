/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.3

Template {
    objectName: "ubuntuListViewTemplate"

    TemplateSection {
        title: "Ubuntu ListView"
        className: "UbuntuListView"

        ListModel {
            id: fruitModel
            objectName: "list_model"
            ListElement { name: "Orange"; details: "The orange (specifically, the sweet orange) is the fruit of the citrus species Citrus × ​sinensis in the family Rutaceae. The fruit of the Citrus sinensis is called sweet orange to distinguish it from that of the Citrus aurantium, the bitter orange. The orange is a hybrid, possibly between pomelo (Citrus maxima) and mandarin (Citrus reticulata), cultivated since ancient times.\n-\nWikipedia"}
            ListElement { name: "Apple"; details: "Fruit" }
            ListElement { name: "Tomato"; details: "The tomato is the edible, often red fruit of the plant Solanum lycopersicum, commonly known as a tomato plant. Both the species and its use as a food originated in Mexico, and spread throughout the world following the Spanish colonization of the Americas. Its many varieties are now widely grown, sometimes in greenhouses in cooler climates. The tomato is consumed in diverse ways, including raw, as an ingredient in many dishes, sauces, salads, and drinks. While it is botanically a fruit, it is considered a vegetable for culinary purposes (as well as under U.S. customs regulations, see Nix v. Hedden), which has caused some confusion. The fruit is rich in lycopene, which may have beneficial health effects. The tomato belongs to the nightshade family (Solanaceae). The plants typically grow to 1–3 meters (3–10 ft) in height and have a weak stem that often sprawls over the ground and vines over other plants. It is a perennial in its native habitat, although often grown outdoors in temperate climates as an annual. An average common tomato weighs approximately 100 grams (4 oz).\n-\nWikipedia" }
            ListElement { name: "Carrot"; details: "Vegetable" }
            ListElement { name: "Potato"; details: "Vegetable" }
            ListElement { name: "Mango"; details: "Fruit" }
            ListElement { name: "Banana"; details: "Fruit" }

            property bool refreshing: refreshTimer.running
            function refresh() {
                refreshTimer.restart();
            }
        }
        Timer {
            id: refreshTimer
            interval: 400
        }

        UbuntuListView {
            objectName: "ubuntuListView"
            width: parent.width
            height: units.gu(36)
            model: fruitModel
            clip: true
            currentIndex: -1

            delegate: ListItem {
                ListItemLayout {
                    title.text: name
                }
                // to enable highlight
                onClicked: ListView.view.currentIndex = index
            }

            pullToRefresh {
                enabled: true
                onRefresh: model.refresh()
                refreshing: model.refreshing
            }
        }
    }
}
