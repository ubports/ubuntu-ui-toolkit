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
                model: 15
                AnimationCircle {
                    radius: 10

                    SequentialAnimation on opacity {
                        loops: Animation.Infinite
                        PauseAnimation {
                            duration: Math.random() * 500
                        }
                        UbuntuNumberAnimation {
                            from: 0.0
                            to: 1.0
                            duration: UbuntuAnimation.SnapDuration
                        }
                    }
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Fast")

            AnimationCircle {
                radius: 100

                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    PauseAnimation {
                        duration: 500
                    }
                    UbuntuNumberAnimation {
                        from: 0.0
                        to: 1.0
                        duration: UbuntuAnimation.FastDuration
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
                        to: 50
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
                        from: rotation
                        to: rotation + 250
                        duration: UbuntuAnimation.SleepyDuration
                    }
                }
            }
        }
    }
}
