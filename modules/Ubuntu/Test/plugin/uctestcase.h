/*
 * Copyright 2013-2014 Canonical Ltd.
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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#ifndef UBUNTU_TEST_UBUNTUTESTCASE_H
#define UBUNTU_TEST_UBUNTUTESTCASE_H

#include <QtTest/QtTest>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include <QtTest/QSignalSpy>

class UbuntuTestCase : public QQuickView
{
    Q_OBJECT

public:
    UbuntuTestCase(const QString& file, QWindow* parent = 0);
    int warnings() const;
    // getter
    template<class T>
    inline T findItem(const QString& objectName) const {
        T item = rootObject()->findChild<T>(objectName);
        if (item)
            return item;
        if (rootObject()->findChild<QObject*>(objectName))
            qFatal("Item '%s' found with unexpected type", qPrintable(objectName));
        qFatal("No item '%s' found", qPrintable(objectName));
    }

private:
    QSignalSpy* m_spy;
};

#endif // UBUNTU_TEST_UBUNTUTESTCASE_H
