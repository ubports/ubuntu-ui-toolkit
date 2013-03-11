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

#ifndef ITEMSTYLEATTACHED_P_H
#define ITEMSTYLEATTACHED_P_H

#include "itemstyleattached.h"
#include "stylecache_p.h"

class QQmlContext;
class ItemStyleAttachedPrivate {
    Q_DECLARE_PUBLIC(ItemStyleAttached)
public:
    ItemStyleAttachedPrivate(ItemStyleAttached *qq, QObject *attached);
    ~ItemStyleAttachedPrivate();

    ItemStyleAttached *q_ptr;
    QQuickItem *attachee;
    QObject *style;
    QQuickItem *delegate;
    QString styleClass;
    QString styleId;
    Selector styleSelector;

    // internal members
    QQmlContext *componentContext;
    StyleCache::StyleData *styleRule;
    QString stylableProperties;
    QString connectedToAttachee;
    bool propertyUpdate;
    bool delayApplyingStyle;
    bool customStyle;
    bool customDelegate;
    bool connectedToEngine;


    void watchAttacheeProperties();
    void bindStyleWithAttachee();
    void bindStyleWithDelegate();
    void connectStyleToAttachee(int styleIndex);
    void disconnectStyleFromAttachee(const QString &property);
    bool updateStyleSelector();
    bool updateStyle();
    bool updateDelegate();
    void updateCurrentStyle();
    bool registerName(const QString &id);
    void listenThemeEngine();
    void _q_attacheePropertyChanged();
    void _q_updateAttacheeProperty();
    void _q_updateDelegateProperty();
    void _q_refreshStyle();
    void _q_reapplyStyling(QQuickItem *);

};

#endif // ITEMSTYLEATTACHED_P_H
