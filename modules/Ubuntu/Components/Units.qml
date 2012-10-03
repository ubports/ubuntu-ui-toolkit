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

import QtQuick 1.1

QtObject {
    id: units

    property real density: 113
    property string formFactor: "desktop"
    property int defaultBucket: 1
    property variant densityBuckets: [["ldpi", 120, 0.75],
                                      ["mdpi", 160, 1.0],
                                      ["hdpi", 240, 1.5],
                                      ["xhdpi", 320, 2.0]]
    property int bucket
    bucket: {
        var i, bucketDensity, delta
        var previousDelta = Infinity
        var distanceIndependentDensity

        if (formFactor == "desktop") {
            distanceIndependentDensity = density * 1.45
        } else {
            distanceIndependentDensity = density
        }

        for (i=0; i<densityBuckets.length; i++) {
            bucketDensity = densityBuckets[i][1]
            delta = Math.abs(bucketDensity - distanceIndependentDensity)
            if (previousDelta < delta) break
            previousDelta = delta
        }
        return i-1
    }

    property real scaleFactor: bucketScaleFactor
    property real bucketScaleFactor: densityBuckets[bucket][2] / densityBuckets[defaultBucket][2]
    property string bucketName: densityBuckets[bucket][0]

    function dp(x) {
        return Math.floor(x * scaleFactor)
    }
}
