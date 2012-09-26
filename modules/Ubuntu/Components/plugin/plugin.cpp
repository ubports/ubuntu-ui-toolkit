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

#include "plugin.h"

#include <QtDeclarative/QDeclarativeExtensionPlugin>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeItem>

#include "style.h"
#include "themeengine.h"
#include "styleditem.h"

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<StyleRule>(uri, 0, 1, "Rule");

    // these need to be registered to QtQuick 1.1 otherwise the revisioned
    // properties won't be accessible
    const char *quick = "QtQuick";
    qmlRegisterType<StyledItem, 1>(quick, 1, 1, "StyledItem");
    qmlRegisterRevision<QDeclarativeItem, 1>(quick, 1, 1);
}

void UbuntuComponentsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
    // add theme engine
    engine->rootContext()->setContextProperty("theme", ThemeEngine::instance());

    ThemeEngine::initialize(engine);
    // small tests, move these to unit test
/*
    StylePath path;
    path.append(StylePathNode("toolbar", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("button", QString(), StylePathNode::Descendant));
    ThemeEngine::instance()->styleRuleForPath(path);

    path.clear();
    path.append(StylePathNode("toolbar", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("column", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("button", QString(), StylePathNode::Descendant));
    ThemeEngine::instance()->styleRuleForPath(path);

    path.clear();
    path.append(StylePathNode("tbox", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("tbutton", QString(), StylePathNode::Descendant));
    ThemeEngine::instance()->styleRuleForPath(path);

    path.clear();
    path.append(StylePathNode("tbox", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("tframe", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("tlayout", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("tbutton", QString(), StylePathNode::Descendant));
    ThemeEngine::instance()->styleRuleForPath(path);

    path.clear();
    path.append(StylePathNode("tbox", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("tframe", QString(), StylePathNode::Descendant));
    path.append(StylePathNode("tbutton", QString(), StylePathNode::Child));
    ThemeEngine::instance()->styleRuleForPath(path);

    ThemeEngine::instance()->setDebug(false);
*/
}

Q_EXPORT_PLUGIN2(UbuntuComponents, UbuntuComponentsPlugin)

