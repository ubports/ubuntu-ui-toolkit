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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 *          Florian Boucault <florian.boucault@canonical.com>
 */

#ifndef THEMEENGINE_H
#define THEMEENGINE_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtQml/QQmlComponent>

#include "themesettings.h"

class ThemeEngine : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
    static ThemeEngine& instance() {
        static ThemeEngine instance;
        return instance;
    }

    explicit ThemeEngine(QObject *parent = 0);

    // getter/setters
    QString name() const;
    void setName(QString name);

    Q_INVOKABLE QQmlComponent* createStyleComponent(QString styleName, QObject* parent);

Q_SIGNALS:
    void nameChanged();

private Q_SLOTS:
    void onThemeNameChanged();
    QUrl pathFromThemeName(QString themeName);
    void updateThemePaths();
    QUrl styleUrlForTheme(QString styleName);
    QString parentThemeName(QString themeName);
private:
    QString m_name;
    QList<QUrl> m_themePaths;
    ThemeSettings m_themeSettings;
};

#endif // THEMEENGINE_H
