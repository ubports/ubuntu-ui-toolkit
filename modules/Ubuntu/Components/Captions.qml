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

/*!
  \qmltype Captions
  \inqmlmodule Ubuntu.Components 1.2
  \ingroup unstable-ubuntu-listitems
  \since Ubuntu.Components 1.2
  \brief Container providing presets for a twin-label column that can be used in
  \l ListItemLayout or RowLayout, ColumnLayout and GridLayout.

  The labels are placed in a column and can be accessed through \l title and
  \l subtitle properties. The default spacing between the labels is driven by the
  amount of lines to be displayed in the \l subtitle label, however it can also
  be configured through the \l layout property.

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

  Additional items can also be added to the layout after the two labels. These
  can be simply declared as child items to the \l layout property.
  \qml
  Captions {
      title.text: "Caption"
      subtitle.text: "Subtitle"
      layout.data: [
          Label {
              text: "third line"
              fontSize: "x-small"
          }
      ]
  }
  \endqml

  The Captions is not visible if neither the label content is set. Altering \b
  visible property may destroy this handling. To preserve visibility control use
  PropertyChanges and states when altering visible property.
  \qml
  import QtQuick 2.4
  import QtQuick.Layouts 1.1
  import Ubuntu.Components 1.2

  ListItem {
      width: units.gu(40)
      height: units.gu(71)

      Captions {
          enabled: true
          onClicked: details.state = (details.state == "hidden") ? "" : "hidden"
          title.text: "Caption text"
      }
      Captions {
          id: details
          preset: "summary"
          title.text: "summary"
          states: State {
              name: "hidden"
              PropertyChanges {
                  target: details
                  visible: false
              }
          }
      }
  }
  \endqml

  When used with \l ListItem, there can be situations when tapping or clicking
  over the Captions requires a different action than when clicking on the rest
  of the \l ListItem. This can be achieved by simply placing a MouseArea as child
  of Captions. In this case \l ListItem will not emit the \l {ListItem::clicked}
  {clicked()} signal when clicked/tapped over the Captions area.
  \qml
  ListItem {
      ListItemLayout {
          Captions {
              title.text: "Caption (title) click on captions and"
              subtitle.text: longText
              MouseArea {
                  anchors.fill: parent
                  onClicked: console.log("Captions clicked");
              }
          }
          Captions {
              preset: "summary"
              title.text: "Text"
              subtitle.text: "Text"
          }
      }
    onClicked: console.log("ListItem clicked")
  }
  \endqml
  */
Item {
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

    /*!
      \qmlproperty Column layout
      \readonly
      Property holding the layout. The property can be used to add additional
      components or to alter spacing used in layout.
      */
    property alias layout: layoutContainer

    clip: true
    visible: (title.text !== "" || subtitle.text !== "")
    Layout.fillWidth: (preset === "caption")
    Layout.alignment: Qt.AlignVCenter | ((preset === "summary") ? Qt.AlignRight : Qt.AlignLeft)
    Layout.minimumWidth: 0
    Layout.maximumWidth: (preset === "summary") ? units.gu(6) : parent.width
    Layout.preferredWidth: (preset === "summary") ? Layout.maximumWidth : 0
    Layout.minimumHeight: 0
    Layout.maximumHeight: parent.height
    Layout.preferredHeight: childrenRect.height

    Column {
        id: layoutContainer
        anchors {
            left: parent.left
            right: parent.right
        }
        height: childrenRect.height

        clip: true
        spacing: units.gu(0.5)

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
            fontSize: "x-small"
            horizontalAlignment: (preset === "summary") ? Text.AlignRight : Text.AlignLeft
            visible: text !== ""
            maximumLineCount: (preset === "caption") ? 2 : 1
            wrapMode: (preset === "caption") ? Text.Wrap : Text.NoWrap
            elide: (preset === "caption") ? Text.ElideRight : Text.ElideNone
        }
    }
}
