/*
 * Copyright 2014 Canonical Ltd.
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

#include "uctestcase.h"
#include "adapters/dbuspropertywatcher_p.h"
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

class tst_DBusPropertyWatcher : public QObject
{
    Q_OBJECT

public:
    tst_DBusPropertyWatcher() {}

private:

private Q_SLOTS:

    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

};

QTEST_MAIN(tst_DBusPropertyWatcher)

#include "tst_dbuswatcher.moc"
