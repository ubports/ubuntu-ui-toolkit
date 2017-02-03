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

#include <QtQml/QQmlEngine>
#include <QtQuick/private/qquickimagebase_p.h>
#include <QtTest/QtTest>
#include <UbuntuToolkit/ubuntutoolkitmodule.h>
#define protected public
#include <UbuntuToolkit/private/ucqquickimageextension_p.h>
#undef protected
#include <QtCore/private/qabstractfileengine_p.h>
#include <QQuickView>

UT_USE_NAMESPACE

unsigned int numberOfTemporarySciFiles() {
    QStringList nameFilters;
    nameFilters << "*.sci";
    return QDir::temp().entryList(nameFilters, QDir::Files).count();
}

int nFaces = 0;

class DummyFileEngineHandler : public QAbstractFileEngineHandler
{
public:
    QAbstractFileEngine *create(const QString &fileName) const
    {
        if (fileName.endsWith("/face.png"))
            nFaces++;

        return nullptr;
    }
};

class tst_UCQQuickImageExtension : public QObject
{
    Q_OBJECT

private:
    QQmlEngine *engine = Q_NULLPTR;

private Q_SLOTS:

    void init()
    {
        engine = new QQmlEngine;
        UbuntuToolkitModule::initializeContextProperties(engine);
    }

    void cleanup()
    {
        delete engine;
    }

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
        QString sciFilePath = "data/borderInName.sci";
        QString result;

        QTextStream resultStream(&result);
        image.rewriteSciFile(sciFilePath, "1", resultStream);

        QString expected;
        QTextStream expectedStream(&expected);
        expectedStream << "source: \"image://scaling/1/data/borderInName.png\"" << endl;
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
        QUrl sciFileUrl = QUrl::fromLocalFile("./data/test.sci");

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

    void onlyOneStatRepeatedImage() {
        DummyFileEngineHandler handler;

        QQuickView view(QUrl::fromLocalFile("./data/hundred_faces.qml"));

        // Wait until the app is idle
        bool idle = false;
        while (!idle) {
            QEventLoop l;
            idle = !l.processEvents();
        }
        // We have actually loaded the image
        QVERIFY(nFaces > 0);
        // The number at the moment of writing this test is 4, but we
        // can't know what Qt does internally, so i'm going for a relatively safe <10
        // the important part is that is not 100 times (i.e. one for every Image item)
        QVERIFY(nFaces < 10);
    }
};

QTEST_MAIN(tst_UCQQuickImageExtension)

#include "tst_qquick_image_extension.moc"
