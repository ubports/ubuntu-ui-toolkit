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
class QQmlAbstractBinding;
class UCTheme : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(UCTheme *parentTheme READ parentTheme NOTIFY parentThemeChanged FINAL)
    Q_PROPERTY(QString name READ name WRITE setName RESET resetName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QObject* palette READ palette WRITE setPalette RESET resetPalette NOTIFY paletteChanged FINAL)
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
    void setPalette(QObject *config);
    void resetPalette();

    Q_INVOKABLE QQmlComponent* createStyleComponent(const QString& styleName, QObject* parent);
    static void registerToContext(QQmlContext* context);

    // helper functions
    QColor getPaletteColor(const char *profile, const char *color);

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
    void updateEnginePaths();
    void onThemeNameChanged();
    void updateThemePaths();
    QUrl styleUrl(const QString& styleName);
    void loadPalette(bool notify = true);
    void _q_configurePalette(bool notify = true);

private:
    UCTheme(bool defaultStyle, QObject *parent = 0);
    void init();
    void applyPaletteConfiguration(const QString &valueSet);
    void restorePalette();

    class PaletteBinding
    {
    public:
        PaletteBinding(const QString &property, const QVariant &value)
            : isValue(true), property(property), value(value), binding(0)
        {}
        PaletteBinding(const QString &property, QQmlAbstractBinding *binding)
            : isValue(false), property(property), binding(binding)
        {}

        bool isValue;
        QString property;
        QVariant value;
        QQmlAbstractBinding *binding;
    };

    QString m_name;
    QPointer<QObject> m_palette; // the palette might be from the default style if the theme doesn't define palette
    QQmlEngine *m_engine;
    QObject *m_paletteConfiguration;
    QList<QUrl> m_themePaths;
    QList<PaletteBinding> m_bindingList;
    UCDefaultTheme m_defaultTheme;
    bool m_defaultStyle:1;
    bool m_completed:1;

    friend class UCDeprecatedTheme;
};

QUrl pathFromThemeName(QString themeName);

#endif // UCTHEME_H
