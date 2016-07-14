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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include <QtTest/QtTest>
#include <UbuntuToolkit/private/ucscalingimageprovider_p.h>

UT_USE_NAMESPACE

class tst_UCScalingImageProvider: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void scaleToHalfSize() {
        UCScalingImageProvider provider;
        QImage result;
        QSize returnedSize;
        QImage expected;

        result = provider.requestImage("0.5/" + QDir::currentPath() + QDir::separator() + "input.png", &returnedSize, QSize());
        expected = QImage("scaled_half.png");

        QCOMPARE(result, expected);
        QCOMPARE(returnedSize, expected.size());
    }

    void qrcSupport() {
        UCScalingImageProvider provider;
        QImage result;
        QSize returnedSize;
        QImage expected;

        result = provider.requestImage("0.5/:/test/prefix/input.png", &returnedSize, QSize());
        expected = QImage("scaled_half.png");

        QCOMPARE(result, expected);
        QCOMPARE(returnedSize, expected.size());
    }

    void respectRequestedSize_data() {
        QTest::addColumn<QString>("inputFile");
        QTest::addColumn<QString>("scalingFactor");
        QTest::addColumn<QSize>("requestedSize");
        QTest::addColumn<QSize>("returnedSize");
        QTest::addColumn<QSize>("resultSize");

        QString inputFile(QDir::currentPath() + QDir::separator() + "input128x256.png");
        QTest::newRow("no scaling, bigger width and height") << inputFile << "1.0" << QSize(1000, 1000) << QSize(128, 256) << QSize(128, 256);
        QTest::newRow("no scaling, smaller width")           << inputFile << "1.0" << QSize(50, 1000) << QSize(128, 256) << QSize(50, 100);
        QTest::newRow("no scaling, smaller height")          << inputFile << "1.0" << QSize(1000, 50) << QSize(128, 256) << QSize(25, 50);
        QTest::newRow("no scaling, smaller width and height")<< inputFile << "1.0" << QSize(50, 50) << QSize(128, 256) << QSize(25, 50);
        QTest::newRow("downscaling, bigger width and height")<< inputFile << "0.5" << QSize(1000, 1000) << QSize(64, 128) << QSize(64, 128);
        QTest::newRow("downscaling, smaller width")          << inputFile << "0.5" << QSize(50, 1000) << QSize(64, 128) << QSize(50, 100);
        QTest::newRow("downscaling, smaller height")         << inputFile << "0.5" << QSize(1000, 50) << QSize(64, 128) << QSize(25, 50);
        QTest::newRow("downscaling, smaller width and height")<< inputFile << "0.5" << QSize(50, 50) << QSize(64, 128) << QSize(25, 50);
        QTest::newRow("upscaling, bigger width and height")  << inputFile << "2.0" << QSize(1000, 1000) << QSize(256, 512) << QSize(256, 512);
        QTest::newRow("upscaling, smaller width")            << inputFile << "2.0" << QSize(50, 1000) << QSize(256, 512) << QSize(50, 100);
        QTest::newRow("upscaling, smaller height")           << inputFile << "2.0" << QSize(1000, 50) << QSize(256, 512) << QSize(25, 50);
        QTest::newRow("upscaling, smaller width and height") << inputFile << "2.0" << QSize(50, 50) << QSize(256, 512) << QSize(25, 50);
        QTest::newRow("upscaling, smaller width and no height") << inputFile << "2.0" << QSize(50, 0) << QSize(256, 512) << QSize(50, 100);
        QTest::newRow("upscaling, smaller height and no width") << inputFile << "2.0" << QSize(0, 50) << QSize(256, 512) << QSize(25, 50);
    }

    void respectRequestedSize() {
        UCScalingImageProvider provider;
        QImage result;
        QSize size;

        QFETCH(QString, inputFile);
        QFETCH(QString, scalingFactor);
        QFETCH(QSize, requestedSize);
        QFETCH(QSize, returnedSize);
        QFETCH(QSize, resultSize);

        result = provider.requestImage(scalingFactor + "/" + inputFile, &size, requestedSize);

        QCOMPARE(size, returnedSize);
        QCOMPARE(result.size(), resultSize);
    }
};

QTEST_MAIN(tst_UCScalingImageProvider)

#include "tst_scaling_image_provider.moc"
