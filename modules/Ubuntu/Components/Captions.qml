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
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.2

/*!
  \qmltype Captions
  \inqmlmodule Ubuntu.Components 1.2
  \ingroup unstable-ubuntu-listitems
  \since Ubuntu.Components 1.2
  \brief Container providing presets for a twin-label column that can be used in
  \l ListItemLayout, RowLayout or GridLayout.

  The labels are placed in a column and can be accessed through \l title and
  \l subtitle properties. The default spacing between the labels is 0.5 grid units.

  The container only shows the labels whos text is set to a valid string. The
  labels not having any text set are not occupying the space. When embedded in a
  positioner or in a \l ListItemLayout, the container is aligned vertically centered.

  \qml
  import QtQuick 2.4
  import Ubuntu.Components 1.2

  ListItem {
      ListLayout {
           Captions {
               title.text: "Caption"
               subtitle.text: "Subtitle text"
               Component.onCompleted: subtitle.Layout.alignment = Qt.AlignRight
           }
           Captions {
               preset: "summary"
               title.text: "Text"
               subtitle.text: "Text"
           }
      }
  }
  \endqml

  Additional items can also be added to the layout after the two labels.
  \qml
  Captions {
      title.text: "Caption"
      subtitle.text: "Subtitle"
      Label {
          text: "third line"
          fontSize: "xx-small"
      }
  }
  \endqml
  */
ColumnLayout {
    id: captions

    /*!
      The property configures the arrangement and font sizes of the Labels in the
      component. It can take the following values:
      \list
        \li \b caption - (default) typical configuration for a left aligned twin-label
            setup, where the text covers the remaining area on a list layout.
        \li \b summary - configuration for a right-aligned twin label setup, with
            6 grid units width.
      \endlist
      */
    property string preset: "caption"

    /*!
      \qmlproperty Label title
      \readonly
      Label occupying the top area of the container.
      */
    property alias title: titleLabel

    /*!
      \qmlproperty Label subtitle
      \readonly
      Label occupying the bottom area of the container.
      */
    property alias subtitle: subtitleLabel

    clip: true
    spacing: units.gu(0.5)
    Layout.fillWidth: (preset === "caption")
    Layout.alignment: Qt.AlignVCenter | ((preset === "summary") ? Qt.AlignRight : Qt.AlignLeft)
    Layout.minimumWidth: 0
    Layout.maximumWidth: (preset === "summary") ? units.gu(6) : parent.width
    Layout.preferredWidth: (preset === "summary") ? Layout.maximumWidth : 0
    Layout.minimumHeight: 0
    Layout.maximumHeight: parent ? parent.height : childrenRect.height
    Layout.preferredHeight: childrenRect.height

    // handle visibility, do not override visible property!
    // 'hide' column if title and subtitle is an empty string and
    // there's no more children added
    states: State {
        name: "__q_invisible"
        when: (title.text === "" && subtitle.text === "") && (captions.children.length == 2)
        PropertyChanges {
            target: captions
            visible: false
        }
    }

    Label {
        id: titleLabel
        anchors {
            left: parent.left
            right: parent.right
        }
        fontSize: (preset === "summary") ? "small" : "medium"
        horizontalAlignment: (preset === "summary") ? Text.AlignRight : Text.AlignLeft
        visible: text !== ""
        elide: (preset === "caption") ? Text.ElideRight : Text.ElideNone
    }
    Label {
        id: subtitleLabel
        anchors {
            left: parent.left
            right: parent.right
        }
        fontSize: "small"
        horizontalAlignment: (preset === "summary") ? Text.AlignRight : Text.AlignLeft
        visible: text !== ""
        maximumLineCount: (preset === "caption") ? 2 : 1
        wrapMode: (preset === "caption") ? Text.Wrap : Text.NoWrap
        elide: (preset === "caption") ? Text.ElideRight : Text.ElideNone
    }
}
