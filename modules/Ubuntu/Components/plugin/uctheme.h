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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 *          Florian Boucault <florian.boucault@canonical.com>
 */

#ifndef UCTHEME_H
#define UCTHEME_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlParserStatus>

#include "ucdefaulttheme.h"

class UCStyledItemBase;
class UCTheme : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(UCTheme *parentTheme READ parentTheme NOTIFY parentThemeChanged)
    Q_PROPERTY(QString name READ name WRITE setName RESET resetName NOTIFY nameChanged)
    Q_PROPERTY(QObject* palette READ palette NOTIFY paletteChanged)
public:
    explicit UCTheme(QObject *parent = 0);
    static UCTheme &defaultTheme()
    {
        static UCTheme instance(true);
        return instance;
    }

    // getter/setters
    UCTheme *parentTheme();
    QString name() const;
    void setName(const QString& name);
    void resetName();
    QObject* palette();

    Q_INVOKABLE QQmlComponent* createStyleComponent(const QString& styleName, QObject* parent);
    static void registerToContext(QQmlContext* context);

Q_SIGNALS:
    void parentThemeChanged();
    void nameChanged();
    void paletteChanged();

protected:
    void classBegin();
    void componentComplete()
    {
        m_completed = true;
    }

private Q_SLOTS:
    void _q_defaultThemeChanged();

private:
    UCTheme(bool defaultStyle, QObject *parent = 0);
    void init();
    void updateEnginePaths();
    void updateThemePaths();
    QUrl styleUrl(const QString& styleName);
    void loadPalette(bool notify = true);

    QString m_name;
    QPointer<QObject> m_palette; // the palette might be from the default style if the theme doesn't define palette
    QQmlEngine *m_engine;
    QList<QUrl> m_themePaths;
    UCDefaultTheme m_defaultTheme;
    bool m_defaultStyle:1;
    bool m_completed:1;

    friend class UCDeprecatedTheme;
};

QUrl pathFromThemeName(QString themeName);

#endif // UCTHEME_H
