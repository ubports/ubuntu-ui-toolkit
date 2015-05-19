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
#include <QQmlEngine>
#include <QQmlComponent>

#include <QDir>
#include <QUrl>
#include <ucnamespace.h>

class tst_components_benchmark: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void benchmark_creation_components_data() {
        QTest::addColumn<QString>("fileName");

        QDir dir;
        dir.setPath(QString("%1/%2.%3").arg(UBUNTU_COMPONENT_PATH).arg(MAJOR_VERSION(LATEST_UITK_VERSION)).arg(MINOR_VERSION(LATEST_UITK_VERSION)));
        QVERIFY2(dir.exists(), qPrintable(dir.absolutePath()));
        QStringList nameFilters;
        nameFilters << "*.qml";
        dir.setNameFilters(nameFilters);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setSorting(QDir::Size | QDir::Reversed);

        QFileInfoList list = dir.entryInfoList();
        QVERIFY2(list.size(), qPrintable(dir.absolutePath()));

        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            QTest::newRow(fileInfo.fileName().toLatin1()) << fileInfo.fileName();
        }
    }

    void benchmark_creation_components() {
        QFETCH(QString, fileName);

        QQmlComponent component(&engine, fileName);
        QObject *obj = component.create();
        obj->deleteLater();

        QBENCHMARK {
            QObject *obj = component.create();
            obj->deleteLater();
        }
    }
    void benchmark_creation_listitems_data() {
        QTest::addColumn<QString>("fileName");

        QDir dir;
        dir.setPath(QString("%1/ListItems/%2.%3").arg(UBUNTU_COMPONENT_PATH).arg(MAJOR_VERSION(LATEST_UITK_VERSION)).arg(MINOR_VERSION(LATEST_UITK_VERSION)));
        QVERIFY2(dir.exists(), qPrintable(dir.absolutePath()));
        QStringList nameFilters;
        nameFilters << "*.qml";
        dir.setNameFilters(nameFilters);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setSorting(QDir::Size | QDir::Reversed);

        QFileInfoList list = dir.entryInfoList();
        QVERIFY2(list.size(), qPrintable(dir.absolutePath()));

        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            QTest::newRow(fileInfo.fileName().toLatin1()) << fileInfo.fileName();
        }
    }

    void benchmark_creation_listitems() {
        QFETCH(QString, fileName);

        QQmlComponent component(&engine, fileName);
        QObject *obj = component.create();
        obj->deleteLater();

        QBENCHMARK {
            QObject *obj = component.create();
            obj->deleteLater();
        }
    }

private:
    QQmlEngine engine;
};

QTEST_MAIN(tst_components_benchmark)

#include "tst_components_benchmark.moc"
