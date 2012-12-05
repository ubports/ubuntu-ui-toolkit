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

EditorCursorDelegate {
    id: cursor

    property bool startPin: (item.positionProperty === "selectionStart")
    property var editor: item.editor
    property int cursorPosition: editor[item.positionProperty]
    property bool myCursorUpdate: false
    property bool myXYUpdate: false

    onCursorPositionChanged: {
        if (myCursorUpdate)
            return;
        myXYUpdate = true;
        var rect = editor.positionToRectangle(cursorPosition);
        x = rect.x;
        y = rect.y;
        myXYUpdate = false;
    }

    function updateEditorCursor()
    {
        if (myXYUpdate)
            return;
        myCursorUpdate = true;
        var pos = editor.mapFromItem(item, x, y + height / 2)
        if (startPin)
            editor.select(editor.positionAt(pos.x, pos.y), editor.selectionEnd);
        else
            editor.select(editor.selectionStart, editor.positionAt(pos.x, pos.y));
        myCursorUpdate = false;
    }

    onXChanged: updateEditorCursor()
    onYChanged: updateEditorCursor()
    visible: true

    Rectangle {
        id: pinBall
        width: StyleUtils.itemStyleProperty("pinSize", 0)
        height: width
        radius: width
        smooth: true
        color: StyleUtils.itemStyleProperty("pinColor", "black")
        anchors {
            horizontalCenter: cursor.horizontalCenter
            bottom: startPin ? cursor.top : undefined
            top: !startPin ? cursor.bottom : undefined
        }

        MouseArea {
            anchors.fill: parent
            anchors.margins: -StyleUtils.itemStyleProperty("pinSensingOffset", units.dp(3))
            drag {
                target: cursor
                axis: Drag.XandYAxis
            }
        }
    }
}
