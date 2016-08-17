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
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes 1.3

Page {
    id: aboutPage
    theme: ThemeSettings {
        name: "Ubuntu.Components.Themes.SuruDark"
        palette: Palette {
            normal.background: UbuntuColors.slate
        }
    }
    style: Rectangle {
        anchors.fill: parent
        color: theme.palette.normal.background
    }
    header: PageHeader {
        title: i18n.tr("About...")
    }

    Column {
        spacing: units.gu(0.7)
        width: parent.width
        anchors.centerIn: parent
        Repeater {
            model: ListModel {
                ListElement {
                    text: "Copyright 2012-2015 Canonical Ltd.\n\n"
                }
                ListElement {
                    text: "Contributors\n"
                }
                ListElement {
                    text: "Tim Peeters (#t1mp) <tim.peeters@canonical.com>"
                }
                ListElement {
                    text: "Christian Dywan (#kalikiana) <christian.dywan@canonical.com>"
                }
                ListElement {
                    text: "Zsombor Egri (#zsombi) <zsombor.egri@canonical.com>"
                }
                ListElement {
                    text: "Florian Boucault (#Kaleo) <florian.boucault@canonical.com>"
                }
                ListElement {
                    text: "Loic Molinari (#loicm) <loic.molinari@canonical.com>"
                }
                ListElement {
                    text: "Zoltan Balogh (#bzoltan) <zoltan.balogh@canonical.com>"
                }
                ListElement {
                    text: "Benjamin Zeller (#zbenjamin) <benjamin.zeller@canonical.com>"
                }
            }
            Label {
                width: parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                text: model.text
            }
        }
    }

}
