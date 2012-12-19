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
 *
 */
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import "examples.js" as Examples

/*!
  \brief Examples Gallery

  Examples Gallery contains simple snippets and code examples.

  Demonstrates following Ubuntu Components:
     - PageStack
     - ListItems
     - Popups

  Uses:
     - ListView
     - ListModel

 */

PageStack {
    id: pageStack

    width: units.gu(60)
    height: units.gu(80)

    Component.onCompleted: push(mainPage)

    Page {
        id: mainPage
        title: i18n.tr("Examples Gallery")

        Component {
            id: dialogComponent
            Dialog {
                id: dialog
                title: ""
                text: ""
                Button {
                    text: "Close"
                    onClicked: PopupUtils.close(dialog)
                }
            }
        }

        ListView {
            focus: true
            anchors.fill: parent
            model: ExamplesModel { }
            delegate: ListItem.Subtitled {
                progression: true
                text: i18n.tr(title)
                subText: i18n.tr(subTitle)
                onClicked: {
                    if (Examples.checkDepends(depends,depends_packages,mainPage)) {
                        Examples.loadApp(pageStack,example,exampleResources);
                    } else {
                        var errorDialog = dialogComponent.createObject(parent);
                        errorDialog.title = "Missing dependencies"
                        errorDialog.text = Examples.modelArrayToString(depends_packages);
                        errorDialog.show();
                    }
                }

                onPressAndHold: {
                    var aboutDialog = dialogComponent.createObject(parent);
                    aboutDialog.title = i18n.tr(title);
                    aboutDialog.text = "<b>" + i18n.tr(subTitle) + "</b><br /><p>" + i18n.tr(description) + "</p>"
                    aboutDialog.show();
                }

            }

            section.property: "section"
            section.criteria: ViewSection.FullString
            section.delegate: ListItem.Header { text: i18n.tr(section) }
        }
    }
}
