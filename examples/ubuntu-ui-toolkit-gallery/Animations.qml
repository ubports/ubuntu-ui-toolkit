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

            AnimationCircle {
                radius: 100

                SequentialAnimation on x {
                    loops: Animation.Infinite
                    PauseAnimation {
                        duration: 1000
                    }
                    UbuntuNumberAnimation {
                        from: 0
                        to: 50
                    }
                }
            }
        }
    }

    TemplateSection {
        title: i18n.tr("UbuntuAnimation Durations")
        className: "UbuntuAnimation"

        TemplateRow {
            title: i18n.tr("Snap")

            Repeater {
                id: repeaterSnap
                model: 10
                AnimationCircle {
                    radius: 15

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
                model: 4
                AnimationCircle {
                    radius: 100

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
                radius: 100

                SequentialAnimation on x {
                    loops: Animation.Infinite
                    PauseAnimation {
                        duration: 1000
                    }
                    UbuntuNumberAnimation {
                        from: 0
                        to: 250
                        duration: UbuntuAnimation.SlowDuration
                    }
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Sleepy")

            AnimationCircle {
                radius: 250

                SequentialAnimation on x {
                    loops: Animation.Infinite
                    PauseAnimation {
                        duration: 1000
                    }
                    UbuntuNumberAnimation {
                        from: 0
                        to: 250
                        duration: UbuntuAnimation.SleepyDuration
                    }
                }
            }
        }
    }
}
