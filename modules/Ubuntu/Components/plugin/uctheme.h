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

#ifndef UCTHEME_H
#define UCTHEME_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtQml/QQmlComponent>

#include "ucthemesettings.h"

class UCTheme : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QObject* palette READ palette NOTIFY paletteChanged)
public:
    static UCTheme& instance() {
        static UCTheme instance;
        return instance;
    }

    explicit UCTheme(QObject *parent = 0);

    // getter/setters
    QString name() const;
    void setName(QString name);
    QObject* palette() const;

    Q_INVOKABLE QQmlComponent* createStyleComponent(QString styleName, QObject* parent);
    void registerToContext(QQmlContext* context);

Q_SIGNALS:
    void nameChanged();
    void paletteChanged();

private Q_SLOTS:
    void updateEnginePaths();
    void onThemeNameChanged();
    QUrl pathFromThemeName(QString themeName);
    void updateThemePaths();
    QUrl styleUrl(QString styleName);
    QString parentThemeName(QString themeName);
    void loadPalette();

private:
    QString m_name;
    QObject* m_palette;
    QQmlEngine *m_engine;
    QList<QUrl> m_themePaths;
    UCThemeSettings m_themeSettings;
    bool m_engineUpdated;
};

#endif // UCTHEME_H
