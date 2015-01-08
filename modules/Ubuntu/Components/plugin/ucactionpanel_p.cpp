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

#include "uclistitemactions.h"
#include "uclistitemactions_p.h"
#include "uclistitem_p.h"
#include "quickutils.h"
#include "i18n.h"
#include "plugin.h"
#include <QtQml/QQmlInfo>
#include "ucaction.h"
#include "ucunits.h"
#include "uclistitemstyle.h"

UCActionPanel::UCActionPanel(UCListItem *item, bool leading)
    : QObject(item)
    , listItem(item)
    , panelItem(0)
    , status(UCListItem::Disconnected)
    , leading(leading)
    , connected(false)
{

}

UCActionPanel::~UCActionPanel()
{
}

UCListItemAttached *UCActionPanel::attachedObject()
{
    if (!panelItem) {
        return 0;
    }
    return static_cast<UCListItemAttached*>(
                qmlAttachedPropertiesObject<UCListItem>(panelItem, false));
}

void UCActionPanel::createPanel(QQmlComponent *panelDelegate)
{
    if (panelItem || !panelDelegate) {
        return;
    }
    if (!panelDelegate->isError()) {
        QQmlContext *context = new QQmlContext(qmlContext(listItem));
        panelItem = qobject_cast<QQuickItem*>(panelDelegate->beginCreate(context));
        if (!panelItem) {
            qmlInfo(listItem) << UbuntuI18n::tr("Error creating ListItem actions panel");
        } else {
            context->setParent(panelItem);
            QQml_setParent_noEvent(panelItem, listItem);
            panelItem->setParentItem(listItem);
            // create attached property!
            UCListItemAttached *attached = static_cast<UCListItemAttached*>(
                        qmlAttachedPropertiesObject<UCListItem>(panelItem));
            if (!attached->actions()) {
                attached->setList(listItem, leading, true);
            } else {
                // container is set, but we need to emit the signal again so we get the
                // attached props updated for those cases when the attached property is
                // created before the statement above
                Q_EMIT attached->actionsChanged();
            }
            panelDelegate->completeCreate();
        }
    } else {
        qmlInfo(listItem) << panelDelegate->errorString();
    }
}

UCListItemActions *UCActionPanel::actions()
{
    return leading ? UCListItemPrivate::get(listItem)->leadingActions :
                     UCListItemPrivate::get(listItem)->trailingActions;
}

QQuickItem *UCActionPanel::panel() const
{
    return panelItem;
}

bool UCActionPanel::grabPanel(UCActionPanel **panel, UCListItem *item, bool leading)
{
    if (!(*panel) && item) {
        UCListItemActions *actions = leading ? UCListItemPrivate::get(item)->leadingActions :
                                             UCListItemPrivate::get(item)->trailingActions;
        if (actions) {
            (*panel) = new UCActionPanel(item, leading);
            UCListItemPrivate *pItem = UCListItemPrivate::get((*panel)->listItem);
            pItem->initStyleItem();
            if (!pItem->styleItem || (pItem->styleItem && !pItem->styleItem->m_actionsDelegate)) {
                return false;
            }
            (*panel)->createPanel(pItem->styleItem->m_actionsDelegate);
            if (!(*panel)->panelItem) {
                delete (*panel);
                (*panel) = 0;
            }
        }
    }
    if (*panel) {
        (*panel)->connected = true;
        (*panel)->status = leading ? UCListItem::Leading : UCListItem::Trailing;
        Q_EMIT (*panel)->statusChanged();
    }
    return (*panel) != 0;
}

void UCActionPanel::ungrabPanel(UCActionPanel *panel)
{
    if (!panel || !panel->connected) {
        return;
    }
    panel->connected = false;
    panel->panelItem->setVisible(false);
}

bool UCActionPanel::isConnected(UCActionPanel *panel)
{
    return panel && panel->connected;
}
