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

import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    id: overview
    objectName: "overview"

    anchors.fill: parent
    applicationName: "gallery-app"
    automaticOrientation: application.automaticOrientation

    property string mediaCurrentlyInView
    StateSaver.properties: "mediaCurrentlyInView"

    //fullScreen property is used on autopilot tests
    property bool fullScreen: APP.fullScreen

    property alias currentPage: pageStack.currentPage

    Component.onCompleted: {
        if (mediaCurrentlyInView !== "") {
            for (var i = 0; i < MANAGER.mediaLibrary.count; i++) {
                if (MANAGER.mediaLibrary.getAt(i).path == mediaCurrentlyInView) {
                    photoViewerLoader.load();
                    photoViewerLoader.item.animateOpen(MANAGER.mediaLibrary.getAt(i),
                                                       Qt.rect(0,0,0,0));
                    return;
                }
            }
        }

        pageStack.push(tabs);
    }

    function pushPage(page) {
        pageStack.push(page);
    }

    function popPage() {
        pageStack.pop();
    }

    PageStack {
        id: pageStack
        anchors.fill: parent
    }

    Tabs {
        id: tabs
        anchors.fill: parent

        visible: !(photoViewerLoader.item && photoViewerLoader.item.isPoppedUp)

        selectedTabIndex: 1
        StateSaver.properties: "selectedTabIndex"

        onSelectedTabIndexChanged: {
            if (selectedTabIndex == 0)
                albumsCheckerboardLoader.load();
            if (selectedTabIndex == 2)
                photosOverviewLoader.load();
            // prevent leaving views in selection mode
            eventView.leaveSelectionMode();
            photosOverviewLoader.leaveSelectionMode();
        }

        Tab {
            id: albumsTab
            objectName: "albumsTab"
            title: i18n.tr("Albums")
            page: Loader {
                id: albumsCheckerboardLoader
                objectName: "albumsCheckerboardLoader"
                anchors.fill: parent
                asynchronous: true

                function load() {
                    if (source == "")
                        source = "AlbumsOverview.qml"
                }
            }
        }

        Tab {
            id: eventTab
            objectName: "eventsTab"
            title: i18n.tr("Events")
            page: Page {
                id: eventView
                objectName: "organicEventView"

                anchors.fill: parent
                visible: true

            }
        }

        // TODO: Although not using a Loader for the Photo Overview today
        // (see above TODO), will make sense in future when component becomes
        // more heavyweight and causes a longer startup time
        Tab {
            id: photosTab
            title: i18n.tr("Photos")
            objectName: "photosTab"
            page: Loader {
                id: photosOverviewLoader
                anchors.fill: parent

                /// Load the PhotosOverview if not done already
                function load() {
                    if (status === Loader.Null)
                        setSource(Qt.resolvedUrl("PhotosOverview.qml"),
                                  {model: MANAGER.mediaLibrary})
                }

                /// Quit selection mode, and unselect all photos
                function leaveSelectionMode() {
                    if (item)
                        item.leaveSelectionMode();
                }

                Connections {
                    target: photosOverviewLoader.item
                    onMediaSourcePressed: {
                        photoViewerLoader.load();
                        overview.mediaCurrentlyInView = mediaSource.path;

                        var rect = GalleryUtility.translateRect(thumbnailRect,
                                                                photosOverviewLoader,
                                                                photoViewerLoader);
                        photoViewerLoader.item.animateOpen(mediaSource, rect);
                    }
                    onInSelectionModeChanged: {
                        if (photosOverviewLoader.item.inSelectionMode)
                            photosTab.title = i18n.tr("Select")
                        else
                            photosTab.title = i18n.tr("Photos")
                    }
                }
            }
        }
    }

    Item {
        id: albumViewer
        objectName: "albumViewerAnimated"
        anchors.fill: parent
    }

    Item {
        id: albumEditor
        objectName: "albumEditorAnimated"
        anchors.fill: parent
    }

    /// Indicates if the photo viewer is currently open (shown to the user)
    property bool __isPhotoViewerOpen: photoViewerLoader.item && photoViewerLoader.item.isPoppedUp
    Loader {
        id: photoViewerLoader
        objectName: "photoViewerLoader"

        /// is true, if the photoviewer is currently loaded
        property bool loaded: photoViewerLoader.status === Loader.Ready

        function load() {
            if (status === Loader.Null)
                setSource(Qt.resolvedUrl("MediaViewer/PopupPhotoViewer.qml"),
                          {model: MANAGER.mediaLibrary});
        }

        anchors.fill: parent
        z: 100


        Connections {
            target: photoViewerLoader.item
            onCloseRequested: {
                popPage();
                photoViewerLoader.item.fadeClosed();
                overview.mediaCurrentlyInView = "";
            }
        }
    }

    MouseArea {
        id: blocker
        anchors.fill: parent
        enabled: photoViewerLoader.item && photoViewerLoader.item.animationRunning
    }
}
