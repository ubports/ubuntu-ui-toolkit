/*
 * Copyright 2013 Canonical Ltd.
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

#include <QtQuick/QQuickView>

class UbuntuTestCase : public QQuickView
{
    Q_OBJECT

public:
    UbuntuTestCase(const QString& file, QWindow* parent = 0);
    // getter
    QObject* findObject(const QString& objectName) const;
    QQuickItem* findItem(const QString& objectName) const;
    int errorCount() const;

private:
    QString m_language;
};

#endif // UBUNTU_TEST_UBUNTUTESTCASE_H
