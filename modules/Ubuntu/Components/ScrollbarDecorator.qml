import QtQuick 2.0

Item {
    id: root

    property Flickable flickableItem: item.flickableItem
    property bool isVertical: item.__private.vertical

    // These can also be modified (but probably shouldn't)
    property int minimumSize: units.gu(2)

    QtObject {
        id: privateApi
        function canFlick(direction) {
           return item.__private.scrollable && (flickableItem.flickableDirection === direction
                  || flickableItem.flickableDirection === Flickable.HorizontalAndVerticalFlick
                  || flickableItem.flickableDirection === Flickable.AutoFlickDirection);
        }
    }

    // Private stuff
    anchors.fill: parent

    Component {
       id: verticalSizerWrapper
        ScrollDecoratorSizer {
            id: verticalSizer
            positionRatio: flickableItem ? flickableItem.visibleArea.yPosition : 0
            sizeRatio: flickableItem ? flickableItem.visibleArea.heightRatio : 0
            maxPosition: flickableItem ? flickableItem.height : 0
            minSize: minimumSize
        }
   }

    Component {
        id: horizontalSizerWrapper
        ScrollDecoratorSizer {
            id: horizontalSizer
            positionRatio: flickableItem ? flickableItem.visibleArea.xPosition : 0
            sizeRatio: flickableItem ? flickableItem.visibleArea.widthRatio : 0
            maxPosition: flickableItem ? flickableItem.width : 0
            minSize: minimumSize
        }
    }

    Loader {id: verticalSizerLoader}
    Loader {id: horizontalSizerLoader}

    Item {
        id: verticalIndicator
        property bool shown: isVertical && flickableItem != null && privateApi.canFlick(Flickable.VerticalFlick) && (flickableItem.height > 0 && flickableItem.contentHeight > flickableItem.height)
        opacity: shown ? 1.0 : 0.0
        anchors.left: item.__private.leftAnchor(parent)
        anchors.right: item.__private.rightAnchor(parent)
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        onShownChanged: {
            if (shown)
                verticalSizerLoader.sourceComponent = verticalSizerWrapper;
            else
                verticalSizerLoader.sourceComponent = undefined;
        }

        // slider
        Rectangle {
            color: itemStyle.sliderColor
            anchors.right: parent.right
            y: verticalIndicator.shown && verticalSizerLoader.status == Loader.Ready ? verticalSizerLoader.item.position : 0
            width: itemStyle.scrollAreaThickness
            height: verticalIndicator.shown && verticalSizerLoader.status == Loader.Ready ?
                  Math.max((verticalSizerLoader.item.size - parent.anchors.topMargin - parent.anchors.bottomMargin), minimumSize) : 0
        }
    }

    Item {
        id: horizontalIndicator
        property bool shown: !isVertical && flickableItem != null && privateApi.canFlick(Flickable.HorizontalFlick) && (flickableItem.width > 0 && flickableItem.contentWidth > flickableItem.width)
        opacity: shown ? 1.0 : 0.0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: item.__private.topAnchor(parent)
        anchors.bottom: item.__private.bottomAnchor(parent)

        onShownChanged: {
            if (shown)
                horizontalSizerLoader.sourceComponent = horizontalSizerWrapper;
            else
                horizontalSizerLoader.sourceComponent = undefined;
        }

        // slider
        Rectangle {
            color: itemStyle.sliderColor
            anchors.bottom: parent.bottom
            x:     horizontalIndicator.shown && horizontalSizerLoader.status == Loader.Ready ? horizontalSizerLoader.item.position : 0
            width: horizontalIndicator.shown && horizontalSizerLoader.status == Loader.Ready ?
                    horizontalSizerLoader.item.size : 0
            height: itemStyle.scrollAreaThickness
        }
    }
}

