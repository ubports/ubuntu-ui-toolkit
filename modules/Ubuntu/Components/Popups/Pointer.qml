import QtQuick 2.0
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

// internal class, used inside subclasses of PopupBase
Item {
    id: pointer
    Theming.ItemStyle.class: "pointer"

    // Using Item.Rotation does not play well with the
    //  translation that would be needed after rotating.
    property real longAxis
    property real shortAxis

    // up, down, left or right, or none to hide the pointer
    property string direction: "down"

    // rotate pointer 90 degrees
    property bool rotate: (direction === "left" || direction === "right")

    width: rotate ? shortAxis : longAxis
    height: rotate ? longAxis : shortAxis
//    width: longAxis
//    height: shortAxis
}
