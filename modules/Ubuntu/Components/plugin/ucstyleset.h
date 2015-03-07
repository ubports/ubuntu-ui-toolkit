/*
 * Copyright 2013-2015 Canonical Ltd.
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

#ifndef UCSTYLESET_H
#define UCSTYLESET_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlParserStatus>

#include "ucthemesettings.h"

class UCStyleSet : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString name READ name WRITE setName RESET resetName NOTIFY nameChanged)
    Q_PROPERTY(QObject* palette READ palette WRITE setPalette NOTIFY paletteChanged)
public:
    explicit UCStyleSet(QObject *parent = 0);
    static UCStyleSet &defaultSet()
    {
        static UCStyleSet instance(true);
        return instance;
    }

    // getter/setters
    QString name() const;
    void setName(const QString& name);
    void resetName();
    QObject* palette();
    void setPalette(QObject *palette);

    Q_INVOKABLE QQmlComponent* createStyleComponent(const QString& styleName, QObject* parent);
    static void registerToContext(QQmlContext* context);

Q_SIGNALS:
    void nameChanged();
    void paletteChanged();

protected:
    void classBegin();
    void componentComplete()
    {
        m_completed = true;
    }

private Q_SLOTS:
    void updateEnginePaths();
    void onThemeNameChanged();
    void updateThemePaths();
    QUrl styleUrl(const QString& styleName);
    QString parentThemeName(const QString& themeName);
    void loadPalette(bool notify = true);

private:
    UCStyleSet(bool defaultStyle);
    void init();

    QString m_name;
    QPointer<QObject> m_palette; // the palette might be from the default style if the theme doesn't define palette
    QQmlEngine *m_engine;
    QList<QUrl> m_themePaths;
    UCThemeSettings m_themeSettings;
    bool m_defaultStyle:1;
    bool m_completed:1;

    friend class UCDeprecatedTheme;
};

QUrl pathFromThemeName(QString themeName);

#endif // UCSTYLESET_H
