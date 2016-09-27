/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components.Styles 1.3 as Style

Style.PullToRefreshStyle {
    id: style
    implicitHeight: refreshIndicatorItem.height + units.gu(5)

    defaultContent: Label {
        id: labelComponent
        visible: control.enabled
        text: releaseToRefresh ? i18n.tr("Release to refresh...") : i18n.tr("Pull to refresh...")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        Behavior on text {
            SequentialAnimation {
                UbuntuNumberAnimation {
                    target: labelComponent
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                }
                UbuntuNumberAnimation {
                    target: labelComponent
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                }
            }
        }
    }

    // additional configuration properties provided by the Ambiance theme
    // these properties can be used by the deriving themes to configure the label
    // and the activity indicator
    property Item label: contentLoader.item
    property alias refreshIndicator: refreshIndicatorItem

    /*
      Local properties
      */
    readonly property PullToRefresh control: styledItem
    // property to control adding of control.height to the Flickable's topMargin.
    property bool extendTopMargin: false
    onExtendTopMarginChanged: {
        if (extendTopMargin) {
            control.target.topMargin += control.height;
        } else {
            control.target.topMargin -= control.height;
        }
    }

    // store when the drag has happened at the beginning of the Flickable's content
    property bool wasAtYBeginning: false
    // initial contentY value when pull started
    property real initialContentY: 0.0
    // drives the refreshing state
    property bool refreshing: false
    // specifies the component completion
    property bool ready: false
    // root item
    property Item rootItem: QuickUtils.rootItem(control)

    anchors.fill: parent

    Component.onCompleted: {
        /*
          When the model attached to the component is refreshing during initialization,
          this refresh will happen after the style gets completed. This refresh will
          cause the style to enter in refreshing state, which alters the topMargin.
          However in the same time the MainView Header will be also updated, so that
          will also alter the topMargin. But when refreshing completes, the topMargin
          will be restored to the default value before the animation, and the content
          will be pushed under the header. We need to connect to the header changes
          so we can reset the state and the topMargin.
        */
        if (rootItem && rootItem.__propagated && rootItem.__propagated.header) {
            rootItem.__propagated.header.visibleChanged.connect(fixTopMargin);
            rootItem.__propagated.header.heightChanged.connect(fixTopMargin);
        }
        ready = true;
    }

    Component.onDestruction: {
        if (rootItem && rootItem.__propagated && rootItem.__propagated.header) {
            rootItem.__propagated.header.visibleChanged.disconnect(fixTopMargin);
            rootItem.__propagated.header.heightChanged.disconnect(fixTopMargin);
        }
    }

    function fixTopMargin() {
        if (style.state === "refreshing") {
            /*
              Fetch the topMargin, force state to disabled (idle will be turned on
              automatically when refreshing completes) and set the topMargin from
              the header change.
              */
            var topMargin = control.target.topMargin;
            style.state = "disabled";
            control.target.topMargin = topMargin;
        }
    }

    // visuals
    Loader {
        id: contentLoader
        sourceComponent: control.content
        onItemChanged: {
            if (item) {
                item.parent = style;
                item.anchors.fill = style;
            }
        }
        asynchronous: false
    }

    ActivityIndicator {
        id: refreshIndicatorItem
        running: false
        anchors.centerIn: parent
    }

    onVisibleChanged: {
        // Updates to contentY may be interrupted if the flickable becomes
        //  invisible (for example when a new Page is pushed on a stack
        //  while PullToRefresh is in refreshing state. See bug #1578619.
        if (visible) {
            // Make sure the flickable is back inside its bounds when
            //  the Page becomes visible again:
            control.target.returnToBounds();
        }
    }

    // state and content controlling
    Connections {
        target: control
        onRefreshingChanged: {
            if (!ready || !control.enabled) {
                return;
            }
            if (!style.releaseToRefresh && target.refreshing) {
                // not a manual refresh
                style.wasAtYBeginning = control.target.atYBeginning;
            }
            /*
              We cannot bind refreshing state activation clause with the
              control.refreshing property dirrectly as when the model is
              refreshed automatically (not manually via the component), we
              need to remember the flickable's topMargin in order to proceed
              with a proper Flickable rebinding. If we use property binding,
              the Connections' onRefreshingChanged will update the flickableTopMargin
              only after the binding is evaluated.
              */
            style.refreshing = target.refreshing;
        }
    }
    Connections {
        target: control.target
        onMovementStarted: {
            style.wasAtYBeginning = control.target.atYBeginning;
            style.initialContentY = control.target.contentY;
            style.refreshing = false;
            style.releaseToRefresh = false;
        }
        onMovementEnded: {
            style.wasAtYBeginning = control.target.atYBeginning;
        }

        // catch when to initiate refresh
        onDraggingChanged: {
            if (!control.parent.dragging && style.releaseToRefresh) {
                style.refreshing = true;
                style.releaseToRefresh = false;
            }
        }
        onContentYChanged: {
            if (style.wasAtYBeginning && control.enabled && control.target.dragging) {
                style.releaseToRefresh = ((style.initialContentY - control.target.contentY) > style.activationThreshold);
            }
        }
    }

    onStateChanged: {
        /*
           Label might not be ready when the component enters in refreshing
           state, therefore the visible property will not be properly returned to
           true. Because of the same reason we cannot have a PropertyChanges either
           as the target is not yet ready at that point.
           */
        if (label) {
            label.visible = (state === "idle" || state === "ready-to-refresh");
        }
    }
    states: [
        State {
            name: "disabled"
            when: !control.enabled
        },
        State {
            name: "idle"
            extend: ""
            when: ready && control.enabled && !style.refreshing && !style.releaseToRefresh
        },
        State {
            name: "ready-to-refresh"
            when: ready && control.enabled && style.releaseToRefresh && !style.refreshing
        },
        State {
            name: "refreshing"
            when: ready && control.enabled && style.wasAtYBeginning && style.refreshing
            PropertyChanges {
                target: refreshIndicatorItem
                running: true
            }
            PropertyChanges {
                target: style
                extendTopMargin: true
            }
        }
    ]

    transitions: [
        Transition {
            from: "ready-to-refresh"
            to: "refreshing"
            SequentialAnimation {
                UbuntuNumberAnimation {
                    target: control.target
                    property: "contentY"
                    to: style.initialContentY - control.height
                }
                ScriptAction {
                    script: control.refresh()
                }
            }
        },
        // transition to be applied when the model is auto-updating
        Transition {
            from: "idle"
            to: "refreshing"
            UbuntuNumberAnimation {
                target: control.target
                property: "contentY"
                to: style.initialContentY - control.height
            }
        },
        Transition {
            from: "refreshing"
            to: "idle"
            UbuntuNumberAnimation {
                target: control.target
                property: "topMargin"
            }
            UbuntuNumberAnimation {
                target: control.target
                property: "contentY"
                to: style.initialContentY
            }
        }
    ]
}

