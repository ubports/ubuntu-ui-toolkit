/*
 * Copyright 2013 Canonical Ltd.
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

Template {
    TemplateSection {
        title: i18n.tr("Ubuntu NumberAnimation")
        className: "UbuntuNumberAnimation"

        TemplateRow {
            title: i18n.tr("Standard")

            Rectangle {
                width: 100
                height: 100
                color: "red"

                UbuntuNumberAnimation on x {
                    from: 0
                    to: 200
                    loops: Animation.Infinite
                }
            }
        }
    }

    TemplateSection {
        title: i18n.tr("UbuntuAnimation Durations")
        className: "UbuntuAnimation"

        TemplateRow {
            title: i18n.tr("Snap")

            Rectangle {
                width: 100
                height: 100
                color: "red"

                NumberAnimation on x {
                    from: 0
                    to: 200
                    loops: Animation.Infinite
                    duration: UbuntuAnimation.SnapDuration
                    easing: UbuntuAnimation.StandardEasing
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Fast")

            Rectangle {
                width: 100
                height: 100
                color: "red"

                NumberAnimation on x {
                    from: 0
                    to: 200
                    loops: Animation.Infinite
                    duration: UbuntuAnimation.FastDuration
                    easing: UbuntuAnimation.StandardEasing
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Slow")

            Rectangle {
                width: 100
                height: 100
                color: "red"

                NumberAnimation on x {
                    from: 0
                    to: 200
                    loops: Animation.Infinite
                    duration: UbuntuAnimation.SlowDuration
                    easing: UbuntuAnimation.StandardEasing
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Sleepy")

            Rectangle {
                width: 100
                height: 100
                color: "red"

                NumberAnimation on x {
                    from: 0
                    to: 200
                    loops: Animation.Infinite
                    duration: UbuntuAnimation.SleepyDuration
                    easing: UbuntuAnimation.StandardEasing
                }
            }
        }
    }
}
