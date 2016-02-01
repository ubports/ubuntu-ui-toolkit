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


/*
 * Test to prevent regressions for bug: https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1338602
 * Activity Indicator crashes in QML/Widget mixed applications
 */

#include <QTest>
#include <QTimer>
#include <QQuickView>
#include <QScopedPointer>
#include <QObject>
#include <QEventLoop>

class Tst_Animator : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void tst_animatorRegression_data() {
        QTest::addColumn<QString>("filename");
        QTest::newRow("1.1") << QString("tst_animator.qml");
        QTest::newRow("1.3") << QString("tst_animator13.qml");
    }

    void tst_animatorRegression ()
    {
        QFETCH(QString, filename);

        QEventLoop l;
        QScopedPointer<QQuickView> w(new QQuickView());
        w->setResizeMode(QQuickView::SizeRootObjectToView);
        w->setSource(QUrl::fromLocalFile(filename));
        w->show();

        int countdown = 20;

        QTimer t;
        t.setInterval(100);
        connect(&t,&QTimer::timeout,[&]{
            w->setVisible(!w->visibility());

            if(--countdown == 0)
                l.exit(0);
        });
        t.start();
        l.exec();
    }

};

QTEST_MAIN(Tst_Animator)
#include "tst_animator.moc"

