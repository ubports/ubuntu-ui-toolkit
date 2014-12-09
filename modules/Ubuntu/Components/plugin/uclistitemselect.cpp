/*
 * Copyright 2014 Canonical Ltd.
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
 */

#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>

UCSelectionHandler::UCSelectionHandler(UCListItem *owner)
    : QObject(owner)
    , listItem(UCListItemPrivate::get(owner))
    , panel(0)
    , selected(false)
    , isConnected(false)
{
}

void UCSelectionHandler::setupPanel(bool animate)
{
    if (panel) {
        return;
    }

    listItem->initStyleItem();
    if (listItem->styleItem && listItem->styleItem->m_selectionDelegate) {
        UCListItem *item = listItem->item();
        if (listItem->styleItem->m_selectionDelegate->isError()) {
            qmlInfo(item) << listItem->styleItem->m_selectionDelegate->errorString();
        } else {
            // create a new context so we can expose context properties
            QQmlContext *context = new QQmlContext(qmlContext(item), item);
            context->setContextProperty("inSelectionMode", animate ? QVariant(false) : isSelectable());
            ContextPropertyChangeListener *listener = new ContextPropertyChangeListener(
                        context, "inSelectionMode");
            listener->setUpdaterProperty(listItem->attachedProperties, "selectable");

            panel = qobject_cast<QQuickItem*>(
                        listItem->styleItem->m_selectionDelegate->beginCreate(context));
            if (panel) {
                QQml_setParent_noEvent(panel, item);
                panel->setParentItem(item);
                // complete component creation
                listItem->styleItem->m_selectionDelegate->completeCreate();
                // turn on selectable
                if (animate) {
                    context->setContextProperty("inSelectionMode", isSelectable());
                }
            }
        }
    }
}

void UCSelectionHandler::getNotified()
{
    if (!listItem->attachedProperties || isConnected) {
        return;
    }
    connect(listItem->attachedProperties, &UCListItemAttached::selectableChanged,
            this, &UCSelectionHandler::setupSelection);
    // also connect to the ListItem's _q_enabler() slot to enable/disable item itself
    connect(listItem->attachedProperties, SIGNAL(selectableChanged()),
            listItem->item(), SLOT(_q_enabler()));
    isConnected = true;
}

// returns true if the ListItem is in selectable mode, false otherwise (also if the
// attached property is NULL)
bool UCSelectionHandler::isSelectable()
{
    UCListItemAttachedPrivate *attached = UCListItemAttachedPrivate::get(listItem->attachedProperties);
    return attached ? attached->isSelectable() : false;
}

bool UCSelectionHandler::isSelected()
{
    return selected;
}

void UCSelectionHandler::setSelected(bool value)
{
    if (selected == value) {
        return;
    }
    selected = value;
    // update attached list
    UCListItem *item = listItem->item();
    if (listItem->attachedProperties) {
        if (selected) {
            UCListItemAttachedPrivate::get(listItem->attachedProperties)->addSelectedItem(item);
        } else {
            UCListItemAttachedPrivate::get(listItem->attachedProperties)->removeSelectedItem(item);
        }
    }
    if (isSelectable()) {
        listItem->update();
    }
    Q_EMIT item->selectedChanged();
}

void UCSelectionHandler::setupSelection()
{
    // make sure the selection mode panel is prepared; selection panel must take care of the visuals
    bool selectable = isSelectable();
    if (selectable) {
        listItem->promptRebound();
        bool animate = (senderSignalIndex() >= 0);
        setupPanel(animate);
    }
    // and finaly update visuals
    listItem->update();
}
