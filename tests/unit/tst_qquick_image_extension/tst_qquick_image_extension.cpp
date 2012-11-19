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
// Make protected methods of QQuickImageExtension public in order to test them
#define protected public
#include "ucqquickimageextension.h"
#undef protected

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
};

QTEST_MAIN(tst_UCQQuickImageExtension)

#include "tst_qquick_image_extension.moc"
