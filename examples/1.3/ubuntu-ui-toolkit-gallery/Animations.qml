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
import Ubuntu.Components 1.3

Template {
    objectName: "animationsTemplate"

    TemplateSection {
        title: i18n.tr("NumberAnimation")
        className: "UbuntuNumberAnimation"

        TemplateRow {
            title: i18n.tr("Standard")

            Item {
                width: units.gu(14)
                height: units.gu(14)

                AnimationCircle {
                    radius: units.gu(6)
                    anchors.centerIn: parent

                    SequentialAnimation on radius {
                        loops: Animation.Infinite
                        PauseAnimation {
                            duration: 1000
                        }
                        UbuntuNumberAnimation {
                            from: units.gu(6)
                            to: units.gu(12)
                        }
                        UbuntuNumberAnimation {
                            from: units.gu(12)
                            to: units.gu(6)
                        }
                    }
                }
            }
        }
    }

    TemplateSection {
        title: i18n.tr("Standard Durations")
        className: "UbuntuAnimation"

        TemplateRow {
            title: i18n.tr("Snap")

            Repeater {
                id: repeaterSnap
                model: 6
                AnimationCircle {
                    radius: units.gu(2)

                    SequentialAnimation on color {
                        PauseAnimation {
                            duration: index * UbuntuAnimation.SnapDuration
                        }
                        SequentialAnimation {
                            loops: Animation.Infinite
                            ColorAnimation {
                                from: "#dd4814"
                                to: "#ddcc14"
                                duration: UbuntuAnimation.SnapDuration
                            }
                            PauseAnimation {
                                duration: repeaterSnap.count * UbuntuAnimation.SnapDuration
                            }
                            ColorAnimation {
                                from: "#ddcc14"
                                to: "#dd4814"
                                duration: UbuntuAnimation.SnapDuration
                            }
                            PauseAnimation {
                                duration: repeaterSnap.count * UbuntuAnimation.SnapDuration
                            }
                        }
                    }
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Fast")

            Repeater {
                id: repeaterFast
                model: 2
                AnimationCircle {
                    radius: units.gu(11)

                    SequentialAnimation on color {
                        PauseAnimation {
                            duration: index * UbuntuAnimation.FastDuration
                        }
                        SequentialAnimation {
                            loops: Animation.Infinite
                            ColorAnimation {
                                from: "#dd4814"
                                to: "#ddcc14"
                                duration: UbuntuAnimation.SnapDuration
                            }
                            PauseAnimation {
                                duration: repeaterFast.count * UbuntuAnimation.FastDuration
                            }
                            ColorAnimation {
                                from: "#ddcc14"
                                to: "#dd4814"
                                duration: UbuntuAnimation.SnapDuration
                            }
                            PauseAnimation {
                                duration: repeaterFast.count * UbuntuAnimation.FastDuration
                            }
                        }
                    }
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Slow")

            AnimationCircle {
                radius: units.gu(12)

                SequentialAnimation on x {
                    loops: Animation.Infinite
                    PauseAnimation {
                        duration: 1000
                    }
                    UbuntuNumberAnimation {
                        from: 0
                        to: units.gu(12)
                        duration: UbuntuAnimation.SlowDuration
                    }
                    PauseAnimation {
                        duration: 300
                    }
                    PropertyAction {
                        value: 0
                    }
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Sleepy")

            Item {
                width: units.gu(10)
                height: units.gu(42)

                AnimationCircle {
                    radius: units.gu(22)

                    SequentialAnimation on y {
                        loops: Animation.Infinite
                        PauseAnimation {
                            duration: 1000
                        }
                        UbuntuNumberAnimation {
                            from: 0
                            to: units.gu(20)
                            duration: UbuntuAnimation.SleepyDuration
                        }
                        PauseAnimation {
                            duration: 300
                        }
                        PropertyAction {
                            value: 0
                        }
                    }
                }
             }
        }
    }
}
