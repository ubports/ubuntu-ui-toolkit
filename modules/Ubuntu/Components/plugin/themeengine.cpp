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

#include "themeengine.h"
#include "style.h"
#include "style_p.h"
#include "styleditem.h"
#include "styleditem_p.h"
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeItem>

#include <QDebug>

Q_GLOBAL_STATIC(ThemeEngine, themeEngine)


ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent),
    themeComponent(0)
{
}

ThemeEngine::~ThemeEngine()
{
    // needed by the Q_GLOBAL_STATIC
}

ThemeEngine* ThemeEngine::instance()
{
    return themeEngine();
}

void ThemeEngine::initialize(QDeclarativeEngine *engine)
{
    ThemeEngine *theme = themeEngine();
    theme->m_engine = engine;
    // load default theme, and build hash tree
    //theme->loadTheme("/usr/lib/qt4/imports/Ubuntu/Components/DefaultTheme.qml");
    theme->loadTheme("modules/Ubuntu/Components/DefaultTheme.qml");
    // load active theme
    theme->loadTheme("demos/ActiveTheme.qml");
    // TODO: watch theme folders to detect system theme changes
}

QDeclarativeItem* ThemeEngine::lookupTarget(const QString &instanceId)
{
    ThemeEngine *engine = themeEngine();
    QDeclarativeItem *ret = 0;
    // TODO: implement
    return ret;
}

Style *ThemeEngine::lookupStyle(StyledItem *item, const QString &state)
{
    ThemeEngine *theme = themeEngine();
    StyledItemPrivate *pitem = item->d_func();
    Style *ret = 0;

    // check whether we have the hierarchy set
    if (pitem->hierarchy.isEmpty()) {
        // build hierarchy now
        pitem->hierarchy = pitem->localSelector;
        for (QDeclarativeItem *pl = item->parentItem(); pl; pl = pl ->parentItem()) {
            StyledItem *plItem = qobject_cast<StyledItem*>(pl);
            if (plItem) {
                // prepend parent's localSelector
                pitem->hierarchy.prepend('.');
                if (plItem->d_ptr->hierarchy.isEmpty())
                    // use localSelector
                    pitem->hierarchy.prepend(plItem->d_ptr->localSelector);
                else {
                    // use parent's hierarchy to speed up
                    pitem->hierarchy.prepend(plItem->d_ptr->hierarchy);
                    // and stop here
                    break;
                }
            }
        }
        // we have the hierarchy, now find a style for it and fix the hierarchy till we find a style
        ret = theme->findStyle(pitem->hierarchy, state);
        while (!ret && !pitem->hierarchy.isEmpty()) {
            // the key is bad, cut from the top till we have what
            int sepIndex = pitem->hierarchy.indexOf('.');
            if (sepIndex >= 0) {
                pitem->hierarchy = pitem->hierarchy.right(
                            pitem->hierarchy.length() - sepIndex - 1);
                // and try to find a style with this
                ret = theme->findStyle(pitem->hierarchy, state);
            } else {
                // there's nothing to cut, so clear the hierarchy, the style was not found
                pitem->hierarchy = QString();
            }
        }

    } else {
        // hierarchy is built and there was a style found based on that, so just lookup for the match
        ret = theme->findStyle(pitem->hierarchy, state);
    }

    return ret;
}

/*!
  \preliminary
  Checks whether the instance can be registerd to the given name, and registers it.
  Removes any previous registration.
*/
bool ThemeEngine::registerInstanceId(StyledItem *item, const QString &newId)
{
    ThemeEngine *engine = themeEngine();
    bool ret = true;

    // check first whether the nex ID is valid and can be registered
    if (newId.isEmpty()) {
        // remove the previous occurence
        engine->m_instanceCache.remove(item->d_ptr->instanceId);
    } else {
        InstanceHash::const_iterator i = engine->m_instanceCache.find(newId);
        if (i.key() == newId)
            ret = false;
        else {
            // remove the previous occurence
            engine->m_instanceCache.remove(item->d_ptr->instanceId);
            // register instance
            engine->m_instanceCache.insert(newId, item);
        }
    }

    return ret;
}


Style *ThemeEngine::findStyle(const QString &key, const QString &state)
{
    QString skey = QString("%1-%2").arg(key).arg(state);
    StyleHash::const_iterator sh = m_styleCache.find(skey);
    if ((sh != m_styleCache.end()) && (sh.key() == skey))
        return sh.value();
    return 0;
}


void ThemeEngine::loadTheme(const QString &themeFile)
{
    themeComponent = new QDeclarativeComponent(m_engine, QUrl::fromLocalFile(themeFile));
    // TODO: do this async!!
    if (themeComponent->isLoading())
        QObject::connect(themeComponent, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
                         this, SLOT(completeThemeLoading()));
    else
        completeThemeLoading();
}

void ThemeEngine::completeThemeLoading()
{
    if (!themeComponent->isError()) {
        QObject *themeObject = themeComponent->create(m_engine->rootContext());
        if (themeObject) {
            // parse its children for Styles
            buildStyleCache(themeObject);

            // DEBUG: print theme hash
            /*
            QHashIterator<QString, Style*> sh(m_styleCache);
            while (sh.hasNext()) {
                sh.next();
                qDebug() << "Style" << sh.key() << "::" << sh.value();
            }
            */
        }
    } else {
        qWarning() << themeComponent->errors();
    }
    // reset component
    themeComponent = 0;
}

// parses theme for style objects
void ThemeEngine::buildStyleCache(QObject *style)
{
    QList<Style*> styles = style->findChildren<Style*>();
    QString selector;
    foreach (Style *pl, styles) {
        // check if we have the style registered already
        selector = prepareStyleSelector(pl);
        // add the style to the cache appending states
        foreach (const QString &state, pl->d_ptr->states) {
            m_styleCache.insert(QString("%1-%2").arg(selector).arg(state), pl);
        }
    }
}

/*
  Prepares the selector for the theme engine based on the hierarchy, class and instanceId.
  This selector will be used by the theme engine style cache.
*/
QString ThemeEngine::prepareStyleSelector(Style *style)
{
    QString selector = style->d_ptr->styleClass;
    if (selector.isEmpty()) {
        // there's no subclass, use Style meta-class name
        selector = style->metaObject()->className();
        selector = selector.left(selector.indexOf("Style_QMLTYPE"));
    }
    // add use instanceId
    if (!style->d_ptr->instanceId.isEmpty()) {
        selector.append(':');
        selector.append(style->d_ptr->instanceId);
    }
    // add the rest of the style parents recursively
    for (QObject *parent = style->parent(); parent; parent = parent->parent()) {
        Style *ps = qobject_cast<Style*>(parent);
        if (ps) {
            selector.prepend('.');
            selector.prepend(prepareStyleSelector(ps));
            break;
        }
    }
    return selector;
}

void ThemeEngine::styledItemPropertiesToSelector(StyledItem *item)
{
    if (!item)
        return;
    ThemeEngine *engine = themeEngine();

    // check if styleClass is specified
    if (!item->d_ptr->styleClass.isEmpty())
        item->d_ptr->localSelector = item->d_ptr->styleClass;
    else {
        // use metaclass name
        item->d_ptr->localSelector = item->metaObject()->className();
        item->d_ptr->localSelector = item->d_ptr->localSelector.left(item->d_ptr->localSelector.indexOf("_QMLTYPE"));
        // update styleClass too so next time we don't need to enter here
        item->d_ptr->styleClass = item->d_ptr->localSelector;
    }
    // check if we have instanceID set
    if (!item->d_ptr->instanceId.isEmpty()) {
        item->d_ptr->localSelector.append(':');
        item->d_ptr->localSelector.append(item->d_ptr->instanceId);
    }

}

void ThemeEngine::styledItemSelectorToProperties(StyledItem *item)
{
    if (!item || (item && item->d_ptr->localSelector.isEmpty()))
        return;
    ThemeEngine *engine = themeEngine();
    // check instanceId
    item->d_ptr->instanceId = item->d_ptr->localSelector.right(
                item->d_ptr->localSelector.length() - item->d_ptr->localSelector.indexOf(':') - 1);
    // set styleClass
    item->d_ptr->styleClass = item->d_ptr->localSelector;
    item->d_ptr->styleClass.remove(item->d_ptr->instanceId);
}

