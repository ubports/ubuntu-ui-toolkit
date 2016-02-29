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

/*!
    \qmltype Sections
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \since Ubuntu.Components 1.3
    \brief Display a list of sections that the user can select. By tapping
        on a section name the \l selectedIndex will be updated, and the
        associated \l Action is triggered.
 */
StyledItem {
    id: sections
    styleName: "SectionsStyle"

    /*!
      List of actions that represent the sections.
      The text of each action is displayed as the section name and clicking
      a section will update the \l selectedIndex.

      When \l selectedIndex is changed (by user interaction or by setting
      the value), actions[selectedIndex] will be triggered.

      Example:
      \qml
        Sections {
            actions: [
                Action {
                    text: "first"
                    onTriggered: print("one")
                },
                Action {
                    text: "second"
                    onTriggered: print("two")
                },
                Action {
                    text: "third"
                    onTriggered: print("three")
                }
            ]
        }
     \endqml
     It is strongly recommended to limit the number of sections to two or three.
     The actions are used as the model for the Sections by default.
     If no trigger functions need to be specified, \l model may be used directly
     without setting the actions property. If both \l actions and \l model are set,
     model overrides the actions.
     */
    property list<Action> actions

    /*!
      The input model for the sections. By default model takes the \l actions
      as input, but if no trigger functions need to be specified, it can be
      simplified to a list of strings naming the sections:
         \qml
            Sections {
                model: [ "one", "two", "three" ]
                onSelectedIndexChanged: {
                    print("Selected section " + model[selectedIndex]);
                }
            }
         \endqml
     */
    property var model: actions
    onModelChanged: {
        if (internal.done) {
            if (!model || model.length === 0) {
                selectedIndex = -1;
            } else if (selectedIndex === 0) {
                // selectedIndex does not change, but action 0 should be triggered.
                internal.triggerAction(0);
            } else {
                // change selectedIndex, which will trigger action 0.
                selectedIndex = 0;
            }
        }
    }

    Component.onCompleted: {
        internal.done = true;
        internal.triggerAction(selectedIndex);
    }
    QtObject {
        id: internal
        property bool done: false

        /*!
          Triggers the action associated with the given
          index, if that action exists.
         */
        function triggerAction(index) {
            if ((index >= 0) && (index < model.length)) {
                if (model[index].hasOwnProperty("trigger")) {
                    model[index].trigger();
                }
            }
        }
    }

    /*!
      The index of the currently selected section in \l model.
      The default value is 0 if there is at least 1 section, or -1 for no sections.
      When the model is changed, selectedIndex is reset to 0 and the first action
      is triggered.
      Upon completion of the Sections component, if there is an Action associated
      with the selected index, that Action will be triggered.
     */
    property int selectedIndex: model ? 0 : -1

    onSelectedIndexChanged: {
        internal.triggerAction(selectedIndex);
    }
}
