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
#include "ucstyle.h"

class QQmlContext;
class ItemStyleAttachedPrivate {
    Q_DECLARE_PUBLIC(ItemStyleAttached)

public:
    ItemStyleAttachedPrivate(ItemStyleAttached *qq, QObject *attached);
    ~ItemStyleAttachedPrivate();

    ItemStyleAttached *q_ptr;
    QQuickItem *attachee;
    UCStyle *style;
    QQuickItem *delegate;
    QString styleClass;
    QString styleId;
    Selector styleSelector;

    // internal members
    QQmlContext *componentContext;
    StyleCache::StyleData *styleRule;
    // hash of attachee property indexes as key, containing enabled/disabled value
    StyledPropertyMap watchedProperties;
    bool delayApplyingStyle;
    bool customStyle;
    bool customDelegate;
    bool connectedToEngine;

    void watchAttacheeProperties();
    bool updateStyle();
    bool updateDelegate();
    void updateTheme();
    void resetStyle();
    void resetDelegate();
    void applyStyleOnChildren(QQuickItem *item);
    bool registerName(const QString &id);
    void listenThemeEngine();
    void _q_attacheePropertyChanged();
    void _q_refreshStyle();
    void _q_reapplyStyling(QQuickItem *);

#ifdef QT_TESTLIB_LIB
    friend class tst_ThemeEngineStyle;
#endif
};

#endif // ITEMSTYLEATTACHED_P_H
