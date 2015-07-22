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
#include <QtQml/QQmlProperty>

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
    Q_PROPERTY(quint16 version READ version WRITE setVersion NOTIFY versionChanged FINAL)
public:
    struct ThemeRecord {
        ThemeRecord() :
            shared(false), deprecated(false)
        {}
        ThemeRecord(const QString &name, const QUrl &path, bool shared, bool deprecated) :
            name(name), path(path), shared(shared), deprecated(deprecated)
        {}
        bool isValid() const
        {
            return path.isValid();
        }

        QString name;
        QUrl path;
        bool shared:1;
        bool deprecated:1;
    };

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
    quint16 version();
    void setVersion(quint16 version);

    // internal, used by the deprecated Theme.createStyledComponent()
    QQmlComponent* createStyleComponent(const QString& styleName, QObject* parent, quint16 version = 0);
    static void registerToContext(QQmlContext* context);

    // helper functions
    QColor getPaletteColor(const char *profile, const char *color);

Q_SIGNALS:
    void parentThemeChanged();
    void nameChanged();
    void paletteChanged();
    void versionChanged();

protected:
    void classBegin();
    void componentComplete()
    {
        m_completed = true;
    }

private Q_SLOTS:
    void resetPalette();
    void _q_defaultThemeChanged();

private:
    UCTheme(bool defaultStyle, QObject *parent = 0);
    void init();
    void updateEnginePaths();
    void updateThemePaths();
    QUrl styleUrl(const QString& styleName, quint16 version, bool *isFallback = NULL);
    void loadPalette(bool notify = true);

    class PaletteConfig
    {
    public:
        PaletteConfig()
            : palette(0), configured(false)
        {}
        ~PaletteConfig()
        {
            restorePalette();
        }

        void configurePalette(QObject *themePalette);
        void restorePalette();
        void reset()
        {
            configList.clear();
        }

        QObject *palette;
    private:
        void buildConfig();
        void apply(QObject *palette);

        struct Data {
            Data(const QString &name, const QQmlProperty &prop)
                : propertyName(name), configProperty(prop), configBinding(0), paletteBinding(0)
            {}
            Data(const QString &name, const QQmlProperty &prop, QQmlAbstractBinding *binding)
                : propertyName(name), configProperty(prop), configBinding(binding), paletteBinding(0)
            {}

            QString propertyName;
            QQmlProperty configProperty;
            QQmlProperty paletteProperty;
            QVariant paletteValue;
            QQmlAbstractBinding *configBinding;
            QQmlAbstractBinding *paletteBinding;
        };

        // configuration palette, not the original theme one
        bool configured:1;
        QList<Data> configList;
    };

    QString m_name;
    QPointer<QObject> m_palette; // the palette might be from the default style if the theme doesn't define palette
    QQmlEngine *m_engine;
    PaletteConfig m_config;
    QList<ThemeRecord> m_themePaths;
    UCDefaultTheme m_defaultTheme;
    quint16 m_version;
    bool m_defaultStyle:1;
    bool m_completed:1;

    friend class UCDeprecatedTheme;
};

UCTheme::ThemeRecord pathFromThemeName(QString themeName);

#endif // UCTHEME_H
