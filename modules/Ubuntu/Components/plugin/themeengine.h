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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#ifndef THEMEENGINE_H
#define THEMEENGINE_H

#include <QObject>
#include <QHash>

class Style;
class QDeclarativeEngine;
class QDeclarativeItem;
class QDeclarativeComponent;
class StyledItem;

typedef QHash<QString, Style*> StyleHash;
typedef QHash<QString, StyledItem*> InstanceHash;
typedef QHashIterator<QString, StyledItem*> InstanceHashIterator;

class ThemeEngine : public QObject
{
    Q_OBJECT
public:
    explicit ThemeEngine(QObject *parent = 0);
    ~ThemeEngine();

    static ThemeEngine* instance();
    static void initialize(QDeclarativeEngine *engine);
    static Style *lookupStyle(StyledItem *item);
    static bool registerInstanceId(StyledItem *item, const QString &newId);

    //utility functions
    static void styledItemPropertiesToSelector(StyledItem *item);
    static void styledItemSelectorToProperties(StyledItem *item);
signals:
    void themeChanged();

public slots:
private slots:
    void completeThemeLoading();

private:
    Style *findStyle(const QString &key);
    void loadTheme(const QString &themeFile);
    void buildStyleCache(QObject *style);
    QString prepareStyleSelector(Style *style);

private: //members
    QDeclarativeEngine *m_engine;
    StyleHash m_styleCache;
    InstanceHash m_instanceCache;
    // needed for theme loading
    QDeclarativeComponent *themeComponent;
};

#endif // THEMEENGINE_H
