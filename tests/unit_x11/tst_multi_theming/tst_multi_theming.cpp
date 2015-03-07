/*
 * Copyright 2015 Canonical Ltd.
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
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include "ucstyleset.h"
#include "uctestcase.h"

class ThemeTestCase : public UbuntuTestCase
{
    Q_OBJECT
public:
    ThemeTestCase(const QString& file, QWindow* parent = 0)
        : UbuntuTestCase(file, parent)
    {
    }

    ~ThemeTestCase()
    {
        // restore theme before quitting
        if (!rootContext()) {
            return;
        }
        UCStyleSet *styleSet = rootStyleSet();
        if (styleSet) {
            styleSet->resetName();
        } else {
            qWarning() << "No theme instance found!";
        }
    }

    void setTheme(const QString &theme)
    {
        rootObject()->setProperty("themeName", theme);
        QTest::waitForEvents();
    }

    void setStyle(const QString &style)
    {
        rootObject()->setProperty("styleDocument", style);
        QTest::waitForEvents();
    }

    UCStyleSet *styleSet()
    {
        return rootObject()->property("styleSet").value<UCStyleSet*>();
    }

    UCStyleSet *rootStyleSet()
    {
        return rootContext()->contextProperty("styleSet").value<UCStyleSet*>();
    }
};

class tst_UCMultiStyling : public QObject
{
    Q_OBJECT
private:
    QString m_xdgDataPath;

private Q_SLOTS:
    void initTestCase()
    {
        m_xdgDataPath = QLatin1String(getenv("XDG_DATA_DIRS"));
    }

    void cleanupTestCase()
    {
        qputenv("XDG_DATA_DIRS", m_xdgDataPath.toLocal8Bit());
    }

};

QTEST_MAIN(tst_UCMultiStyling)

#include "tst_multi_theming.moc"
