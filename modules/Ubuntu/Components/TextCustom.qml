import QtQuick 2.0

/*!
    \qmltype TextCustom
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The TextCustom component is here for backward compatibility,
           please use Label instead
*/

Label {
    Component.onCompleted: console.warn("WARNING: TextCustom component will be deprecated, please use Label instead.")
}
