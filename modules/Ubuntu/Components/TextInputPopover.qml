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
import Ubuntu.Components 0.1 as Toolkit
import "Popups" 0.1

ActionSelectionPopover {
    actions: ActionList {
        Action {
            text: i18n.tr("Select All")
            visible: target && target.text !== "" && target.text !== target.selectedText
            onTriggered: caller.selectAll()
        }
        Action {
            text: i18n.tr("Select Word")
            visible: target && target.text !== "" && target.selectedText === ""
            onTriggered: caller.selectWord()
        }
        Action {
            text: i18n.tr("Cut")
            visible: target && target.selectedText !== ""
            onTriggered: caller.cut()
        }
        Action {
            text: i18n.tr("Copy")
            visible: target && target.selectedText !== ""
            onTriggered: caller.copy()
        }
        Action {
            text: i18n.tr("Paste")
            visible: target && target.canPaste
            onTriggered: caller.paste()
        }
        Action {
            text: i18n.tr("Undo")
            visible: target && target.canUndo
            onTriggered: caller.undo()
        }
        Action {
            text: i18n.tr("Redo")
            visible: target && target.canRedo
            onTriggered: caller.redo()
        }
    }
}

