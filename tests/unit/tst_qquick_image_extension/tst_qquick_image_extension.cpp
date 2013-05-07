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
#include <QtQuick/private/qquickimagebase_p.h>

// Make protected methods of QQuickImageExtension public in order to test them
#define protected public
#include "ucqquickimageextension.h"
#undef protected

unsigned int numberOfTemporarySciFiles() {
    QStringList nameFilters;
    nameFilters << "*.sci";
    return QDir::temp().entryList(nameFilters, QDir::Files).count();
}

class tst_UCQQuickImageExtension : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void scaledBorderIdentity() {
        UCQQuickImageExtension image;
        QString border = "border: 13";
        QString expected = "border: 13";
        QString result = image.scaledBorder(border, "1");
        QCOMPARE(result, expected);
    }

    void scaledBorderHalf() {
        UCQQuickImageExtension image;
        QString border = "border: 13";
        QString expected = "border: 7";
        QString result = image.scaledBorder(border, "0.5");
        QCOMPARE(result, expected);
    }

    void scaledBorderDouble() {
        UCQQuickImageExtension image;
        QString border = "border: 13";
        QString expected = "border: 26";
        QString result = image.scaledBorder(border, "2");
        QCOMPARE(result, expected);
    }

    void rewriteContainsBorderInName() {
        UCQQuickImageExtension image;
        QString sciFilePath = "borderInName.sci";
        QString result;

        QTextStream resultStream(&result);
        image.rewriteSciFile(sciFilePath, "1", resultStream);

        QString expected;
        QTextStream expectedStream(&expected);
        expectedStream << "source: \"image://scaling/1/./borderInName.png\"" << endl;
        expectedStream << "border.left: 9" << endl;
        expectedStream << "border.right: 2" << endl;
        expectedStream << "border.top: 9" << endl;
        expectedStream << "border.bottom: 0" << endl;
        expectedStream << "horizontalTileMode: Stretch" << endl;
        expectedStream << "verticalTileMode: Stretch" << endl;

        QCOMPARE(result, expected);
    }

    void cachingOfRewrittenSciFiles() {
        /* This tests an internal implementation detail of UCQQuickImageExtension,
           namely making sure that only one temporary rewritten .sci file is
           created for each source .sci file.
        */
        QQuickImageBase baseImage;
        UCQQuickImageExtension* image1 = new UCQQuickImageExtension(&baseImage);
        UCQQuickImageExtension* image2 = new UCQQuickImageExtension(&baseImage);
        QUrl sciFileUrl = QUrl::fromLocalFile("./test.sci");

        unsigned int initialNumberOfSciFiles = numberOfTemporarySciFiles();

        image1->setSource(sciFileUrl);
        QCOMPARE(numberOfTemporarySciFiles(), initialNumberOfSciFiles + 1);

        image2->setSource(sciFileUrl);
        QCOMPARE(numberOfTemporarySciFiles(), initialNumberOfSciFiles + 1);

        delete image1;
        QCOMPARE(numberOfTemporarySciFiles(), initialNumberOfSciFiles + 1);

        /* The temporary files will be deleted when the cache is destroyed when
           the application exits.
        */
        delete image2;
        QCOMPARE(numberOfTemporarySciFiles(), initialNumberOfSciFiles + 1);
    }
};

QTEST_MAIN(tst_UCQQuickImageExtension)

#include "tst_qquick_image_extension.moc"
