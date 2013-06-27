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
 */

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include "uctheme.h"


class tst_UCTheme : public QObject
{
    Q_OBJECT

public:
    tst_UCTheme();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_loadTheme();

private:
    QQuickView *m_quickView;
};

tst_UCTheme::tst_UCTheme():
    m_quickView(0)
{
}

void tst_UCTheme::initTestCase()
{
    m_quickView = new QQuickView(0);
    m_quickView->setGeometry(0,0, 240, 320);

    // add modules folder so we have access to the plugin from QML
    QQmlEngine *engine = m_quickView->engine();
    QString modules("../../../modules");
    QStringList imports = engine->importPathList();
    imports.prepend(QDir(modules).absolutePath());
    engine->setImportPathList(imports);
}

void tst_UCTheme::cleanupTestCase()
{
    delete m_quickView;
}

void tst_UCTheme::testCase_loadTheme()
{
}



QTEST_MAIN(tst_UCTheme)

#include "tst_theme_enginetest.moc"
