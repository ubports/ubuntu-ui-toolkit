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
import QtQuick.Window 2.2
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3 as Style

Style.ToolbarStyle {
    id: toolbarStyle
    implicitWidth: parent ? parent.width : 0
    // reduce toolbar height on phone in landscape orientation:
    implicitHeight: Screen.height > units.gu(50) ? units.gu(4) : units.gu(3)

    /*!
      The default action delegate if the styled item does
      not provide a delegate.
     */
    defaultDelegate: AbstractButton {
        style: IconButtonStyle { }
        objectName: action.objectName + "_button"
        height: parent ? parent.height : undefined
        width: units.gu(4)
        action: modelData
    }
}
