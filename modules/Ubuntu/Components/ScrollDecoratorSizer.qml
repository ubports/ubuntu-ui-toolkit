import QtQuick 2.0

QtObject {
    // relative (0..1) position of top and bottom
    property real positionRatio
    property real sizeRatio

    // max position and min size
    property real maxPosition
    property real minSize

    // size underflow
    property real sizeUnderflow: (sizeRatio * maxPosition) < minSize ? minSize - (sizeRatio * maxPosition) : 0

    // raw start and end position considering minimum size
    property real rawStartPos: positionRatio * (maxPosition - sizeUnderflow)
    property real rawEndPos: (positionRatio + sizeRatio) * (maxPosition - sizeUnderflow) + sizeUnderflow

    // overshoot amount at start and end
    property real overshootStart: rawStartPos < 0 ? -rawStartPos : 0
    property real overshootEnd: rawEndPos > maxPosition ? rawEndPos - maxPosition : 0

    // overshoot adjusted start and end
    property real adjStartPos: rawStartPos + overshootStart
    property real adjEndPos: rawEndPos - overshootStart - overshootEnd

    // final position and size of thumb
    property int position: 0.5 + (adjStartPos + minSize > maxPosition ? maxPosition - minSize : adjStartPos)
    property int size: 0.5 + ((adjEndPos - position) < minSize ? minSize : (adjEndPos - position))
}

