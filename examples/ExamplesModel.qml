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
 *
 */
import QtQuick 2.0

ListModel {
    ListElement {
        title: "uChuck"
        subTitle: "Page, Label, UbuntuShape, ActivityIndicator, i18n, JSON, SoundEffect"
        example: "uchuck/uchuck.qml"
        exampleResources: "../resources/"
        section: "Online App"
        description: "A Joke Downloader Page for Examples Gallery. Connects to an online service which returns a JSON from which the joke is parsed into a Label."
        depends: "import QtMultimedia 5.0"
    }
}
