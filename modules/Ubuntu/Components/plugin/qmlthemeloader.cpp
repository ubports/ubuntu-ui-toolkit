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

#include "themeengine.h"
#include "themeengine_p.h"
#include "style.h"
#include "styleditem.h"
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>

/*!
  \internal
  The QmlTheme class loads a QML theme and builds up the ThemeEngine's Rule-element
  suffix-tree.
  */

/*=============================================================================
  QML THEME LOADER
=============================================================================*/
/*!
  \internal
  Loads a QML theme and builds up the style rule tree.
  */
bool QmlTheme::loadTheme(const QUrl &path, QQmlEngine *engine, StyleTreeNode *styleTree)
{
    themeComponent = new QQmlComponent(engine, path);
    this->styleTree = styleTree;
    if (themeComponent->isLoading()) {
        // the QML engine should be initialized by now, so use 0 for accessing instance
        QObject::connect(themeComponent, SIGNAL(statusChanged(QQmlComponent::Status)),
                         ThemeEngine::instance(0), SLOT(_q_continueThemeLoading));
    } else
        return finalizeThemeLoading();
    return true;
}

/*!
  \internal
  Finalizes theme loading. Parses theme for style rules and builds up the cache from it.
*/
bool QmlTheme::finalizeThemeLoading()
{
    bool ret = true;
    if (!themeComponent->isError()) {
        QObject *theme = themeComponent->create();
        if (theme) {
            // parse its children for Styles
            QList<StyleRule*> rules = theme->findChildren<StyleRule*>();

            Q_FOREACH (StyleRule *rule, rules) {
                const QString selector = rule->selector();
                if (selector.isEmpty()) {
                    qWarning() << "Rule without selector!";
                    continue;
                }
                QList<Selector> pathList = ThemeEnginePrivate::parseSelector(selector);
                Q_FOREACH (const Selector &path, pathList) {
                    styleTree->addStyleRule(path, rule);
                }
            }

            styleTree->listTree();
        }
    } else {
        ThemeEnginePrivate::setError(themeComponent->errorString());
        ret = false;
    }

    // reset component
    themeComponent = 0;
    styleTree = 0;
    return ret;
}


