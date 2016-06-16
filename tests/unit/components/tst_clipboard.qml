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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 1.1

TestCase {
    name: "ClipboardAPI"

    function initTestCase() {
        // clear the clipboard
        Clipboard.clear();
    }

    function test_0_formats() {
        compare(Clipboard.data.formats, [], "Empty clipboard formats is empty");
    }

    function test_clipboard_push_data() {
        return [
                    {tag: "text data", property: "text", data: "test string", result: "test string", expectFail: false},
                    {tag: "HTML data", property: "html", data: testHtml, result: testHtml, expectFail: false},
                    {tag: "URL list", property: "urls", data: testUrls, result: testUrls, expectFail: false},
                    {tag: "Color data", property: "color", data: testColor, result: testColor, expectFail: false},
                    {tag: "MIME data1", property: "data", data: mimeData1, result: mimeData1, expectFail: false},
                    // note that result of mimeData2 should be mimeData1 (the returned
                    // list of mime datas is always a single list)
                    {tag: "MIME data2", property: "data", data: mimeData2, result: mimeData1, expectFail: false},
                ];
    }

    function test_clipboard_push(data) {
        if (data.expectFail)
            expectFail("", data);
        Clipboard.clear();
        Clipboard.push(data.data);
        compare(Clipboard.data[data.property], data.result, data.tag);
    }

    function test_clipboard_push_standalone() {
        Clipboard.clear();
        Clipboard.push(standalone);
        compare(Clipboard.data.text, standalone.text, "Standalone text");
        compare(Clipboard.data.html, standalone.html, "Standalone HTML");
        compare(Clipboard.data.urls, standalone.urls, "Standalone URL list");
        compare(Clipboard.data.color, standalone.color, "Standalone color");

        standalone.text = "Modified text";
        Clipboard.push(standalone);
        compare(Clipboard.data.text, standalone.text, "Standalone text");
    }

    function test_push_color() {
        Clipboard.push(testColor);
        compare(Clipboard.data.color, "#ff0000", "Color pushed");
        Clipboard.push(["application/x-color", "green"]);
        compare(Clipboard.data.color, standalone.color, "Color pushed");
    }


    MimeData {
        id: standalone
        text: "Standalone text"
        html: testHtml
        urls: testUrls
        color: "green"
        data: [
            "application/x-color", "blue",
            "text/richtext", "<b>This is a rich text</b>"
        ]
    }

    property string testHtml: "<!DOCTYPE html>
                           <html>
                           <head>
                           <title>Title of the document</title>
                           </head>
                           <body>
                           The content of the document......
                           </body>
                           </html>"
    property color testColor: "red"
    property url testUrl: Qt.resolvedUrl("tst_clipboard.qml")
    property var testUrls: [testUrl, "http://www.canonical.com", "http://www.google.com"]
    property var mimeData1: [
        "text/plain", "this is a plain text",
        "application/x-color", "blue",
        "text/richtext", "<b>This is a rich text</b>"
    ]
    property var mimeData2: [
        ["text/plain", "this is a plain text"],
        ["application/x-color", "blue"],
        ["text/richtext", "<b>This is a rich text</b>"]
    ]
}
