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
 *
 */

#include "uctestcase.h"
#include "private/quickutils_p.h"
#include <QtQml/QQmlEngine>

UT_USE_NAMESPACE

class tst_QuickUtils : public QObject
{
    Q_OBJECT
public:
    tst_QuickUtils() {}

private:
    QString imEnvVar;

private Q_SLOTS:
    void initTestCase()
    {
        imEnvVar = qgetenv("QT_IM_MODULE");
        QQmlEngine *engine = new QQmlEngine(this);
        QuickUtils::instance(engine);
    }
    void cleanupTestCase()
    {
        qputenv("QT_IM_MODULE", imEnvVar.toLocal8Bit());
    }

    void test_inputmethod_data()
    {
        QTest::addColumn<QByteArray>("envVar");

        QTest::newRow("(empty)") << QByteArray("");
        QTest::newRow("ibus") << QByteArray("ibus");
        QTest::newRow("compose") << QByteArray("compose");
        QTest::newRow("none") << QByteArray("none");
    }
    void test_inputmethod()
    {
        QFETCH(QByteArray, envVar);
        qputenv("QT_IM_MODULE", envVar);
        QVERIFY(QuickUtils::instance()->inputMethodProvider().isEmpty());
    }
};

QTEST_MAIN(tst_QuickUtils)

#include "tst_quickutils.moc"
