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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef QMLTHEMELOADER_P_H
#define QMLTHEMELOADER_P_H

#include "themeengine_p.h"
#include "themeloader_p.h"

// QML theme loader
class QmlThemeLoader : public QObject, public ThemeLoader {
    Q_OBJECT
public:
    QmlThemeLoader(QQmlEngine *engine);
    virtual ~QmlThemeLoader(){}
    StyleTreeNode *loadTheme(const QUrl &path);

private Q_SLOTS:
    void finalizeThemeLoading();
private:
    QQmlComponent *themeComponent;
    StyleTreeNode *styleTree;
    bool async;
};



#endif // QMLTHEMELOADER_P_H
