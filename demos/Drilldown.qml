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

import QtQuick 1.1
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
Template {
    title: "Drilldown"

    Item {
        // wrapper item because the template controls the anchors
        anchors.fill: parent

        PageStack {
            id: stack
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }

            width: 300

            initialPage: Page {
                title: "Root page"

                contents: PageList {
                    pageStack: stack
                    Page {
                        title: "Page 1"
                        contents: PageList {
                            pageStack: stack

                            Page {
                                title: "Page 1.1"
                                contents: TextCustom { text: "Example page 1.1" }
                            }
                            Page {
                                title: "Page 1.2"
                                contents: TextCustom { text: "Example page 1.2" }
                            }
                            Page {
                                title: "Page 1.3"
                                contents: TextCustom { text: "Example page 1.3" }
                            }
                        }
                    }
                    Page {
                        title: "Page 2"
                        contents: PageList {
                            pageStack: stack

                            Page {
                                title: "Page 2.1"
                                contents: TextCustom { text: "Example page 2.1" }
                            }
                            Page {
                                title: "Page 2.2"
                                contents: TextCustom { text: "Example page 2.2" }
                            }
                            Page {
                                title: "Page 2.3"
                                contents: TextCustom { text: "Example page 2.3" }
                            }
                        }
                    }
                    Page {
                        title: "Page 3"
                        contents: PageList {
                            pageStack: stack

                            Page {
                                title: "Page 3.1"
                                contents: TextCustom { text: "Example page 3.1" }
                            }
                            Page {
                                title: "Page 3.2"
                                contents: TextCustom { text: "Example page 3.2" }
                            }
                            Page {
                                title: "Page 3.3"
                                contents: TextCustom { text: "Example page 3.3" }
                            }
                        }
                    }
                }
            }
        }
    }
}
