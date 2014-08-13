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

#ifndef TST_ANIMATOR_H
#define TST_ANIMATOR_H

#include <QObject>
#include <QTest>
#include <QEventLoop>

class Tst_Animator : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void tst_animatorRegression ();

private:
    QEventLoop l;

};

#endif // TST_ANIMATOR_H
