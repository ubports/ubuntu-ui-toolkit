import QtQuick 2.4

Item {
    id: root
    width: parent.width
    height: units.gu(7)

    //we can't make this an alias of row.children because we have to make sure the labels are always
    //in the second position
    default property alias slots: row.rowSlots
    property alias titleItem: titleLabel
    property alias subtitleItem: subtitleLabel

    Row {
        id: row
        anchors.fill: parent

        //internals
        property int maxNumberOfLeadingSlots: 1
        property int maxNumberOfTrailingSlots: 2

        property list<Item> rowSlots

        //the Label slot is mandatory according to the UX spec
        property var _label: Item {
            id: labelContainer
            height: childrenRect.height

            anchors.verticalCenter: parent ? parent.verticalCenter : undefined

            //using this instead of RowLayout saves us a lot of creation time
            width: {
                var totalWidth = 0
                //iterate with length caching, for higher perf
                for (var i=0, len = row.rowSlots.length; i<len; i++) {
                    totalWidth += row.rowSlots[i].width
                }
                return root.width - totalWidth
            }

            Text {
                id: titleLabel
                font.weight: Font.Light
                font.pixelSize: FontUtils.sizeToPixels("medium")
                //FIXME: is it already in the palette?
                color: "#525252"
            }

            Text {
                id: subtitleLabel
                anchors.top: titleLabel.baseline
                //FIXME: this is hardcoded by design, do we want it hardcoded?
                anchors.topMargin: units.gu(1)
                font.weight: Font.Light
                font.pixelSize: FontUtils.sizeToPixels("small")
                //FIXME: is it already in the palette?
                color: "#525252"
            }
        }

        function relayout() {

            //PERFORMANCE: This code adds about 150ms to the complex1 benchmark
            var currentNumberOfLeadingSlots = 0
            var currentNumberOfTrailingSlots = 0

            //use fixed size arrays to increase performance
            var leftMost = new Array(maxNumberOfLeadingSlots)
            var rightMost = new Array(maxNumberOfTrailingSlots)

            //NOTE: assigning elements to these JS arrays will reset their "parent" property!
            //and assigning them to Row's children will set their parent back to row

            //don't read ALL the slots if the use defines too many of them, it will slow us down!
            var min = Math.min(row.rowSlots.length, maxNumberOfLeadingSlots + maxNumberOfTrailingSlots)
            for (var i=0; i<min; i++) {
                if (row.rowSlots[i] === undefined) continue

                switch (row.rowSlots[i].position) {
                case "Slot.Leading":
                    if (currentNumberOfLeadingSlots < maxNumberOfLeadingSlots) {
                        leftMost[currentNumberOfLeadingSlots] = row.rowSlots[i]
                        currentNumberOfLeadingSlots++
                    } else {
                        console.log("SlotsLayout: we only currently allow", maxNumberOfLeadingSlots, "leading slots.")
                    }
                    break
                case "Slot.Trailing":
                    if (currentNumberOfTrailingSlots < maxNumberOfTrailingSlots) {
                        rightMost[currentNumberOfTrailingSlots] = row.rowSlots[i]
                        currentNumberOfTrailingSlots++
                    } else {
                        console.log("SlotsLayout: we only currently allow", maxNumberOfTrailingSlots, "trailing slots.")
                    }
                    break
                default:
                    console.log("SlotsLayout: Wrong Slot position or position not found.")
                }
            }

            //NOTE: We can't do this because it would mean that if we go from a situation with
            //valid Slots to a state without valid slots, this block is not evaluated so the
            //Label is not put in the children!
            //only change row's children if it makes sense to do so
            //if (currentNumberOfLeadingSlots + currentNumberOfTrailingSlots !== 0) {
            //add the label
            leftMost[currentNumberOfLeadingSlots] = _label

            //finally, assign the whole array
            row.children = leftMost.concat(rightMost)
            //}
        }

        onRowSlotsChanged: relayout()
    }
}
