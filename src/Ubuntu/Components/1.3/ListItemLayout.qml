import QtQuick 2.4
import Ubuntu.Components 1.3

SlotsLayoutCpp {
    property alias title: captions.title
    property alias subtitle: captions.subtitle
    property alias summary: captions.summary

    mainSlot: ThreeLabelsSlot {
        id: captions
    }
}
