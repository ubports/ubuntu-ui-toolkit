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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include <QtTest/QtTest>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>

class tst_UbuntuShape: public QObject
{
    Q_OBJECT

private:
    QQuickView *m_quickView;

private Q_SLOTS:

    void initTestCase()
    {
        m_quickView = new QQuickView;
        m_quickView->setGeometry(0, 0, 900, 500);
        m_quickView->show();

        // add modules folder so we have access to the plugin from QML
        QQmlEngine *engine = m_quickView->engine();
        QString modules(UBUNTU_QML_IMPORT_PATH);
        QStringList imports = engine->importPathList();
        imports.prepend(QDir(modules).absolutePath());
        engine->setImportPathList(imports);
    }

    void noDistortion() {
        QSKIP("This test passes on local machines but not in CI as it requires working OpenGL");
        m_quickView->setSource(QUrl::fromLocalFile("no_distortion.qml"));
        QCoreApplication::processEvents();

        QImage result = m_quickView->grabWindow();
        QVERIFY(!result.isNull());

        QImage expected = QImage("no_distortion_expected.png");
        QVERIFY(!expected.isNull());

        QCOMPARE(result, expected);
    }
};

QTEST_MAIN(tst_UbuntuShape)

#include "tst_ubuntu_shape.moc"
