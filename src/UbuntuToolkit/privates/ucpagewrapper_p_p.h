/*
 * Copyright 2016 Canonical Ltd.
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

#ifndef UCPAGEWRAPPER_P_H
#define UCPAGEWRAPPER_P_H

#include <ubuntutoolkitglobal.h>
#include "ucpagetreenode_p_p.h"
#include "ucpagewrapper_p.h"

UT_NAMESPACE_BEGIN

class UCPageWrapper;
class UCPageWrapperIncubator;
class UCPageWrapperPrivate : public UCPageTreeNodePrivate
{
    Q_DECLARE_PUBLIC(UCPageWrapper)

public:
    UCPageWrapperPrivate ();
    ~UCPageWrapperPrivate ();
    void init();

    enum PropertyFlags {
        CustomVisible = LastPageTreeNodeFlag
    };

    enum State {
        Waiting, LoadingComponent, CreatingObject, NotifyPageLoaded, Ready, Error
    };

    void initPage();
    void reset ();
    void activate   ();
    void deactivate ();
    QQuickItem *toItem (QObject *theObject, bool canDelete = true);
    void initItem (QQuickItem *theItem);
    void copyProperties (QObject *target);

    void createIncubator  ();
    void destroyIncubator ();
    void onActiveChanged();

    void setCanDestroy(bool canDestroy);

    //state machine functions
    void nextStep ();
    void loadComponentState ();
    void createObjectState ();
    void finalizeObjectIfReady ();

    QVariant m_reference;
    QVariant m_properties;
    QQuickItem* m_object;
    QQuickItem* m_parentPage;
    QQuickItem* m_parentWrapper;
    QQuickItem* m_pageHolder;
    UCPageWrapperIncubator* m_incubator;
    QQmlComponent *m_component;
    QQmlContext *m_itemContext;
    State m_state;
    int m_column;
    bool m_canDestroy:1;
    bool m_synchronous:1;
    bool m_ownsComponent:1;
};

UT_NAMESPACE_END

#endif // UCPAGEWRAPPER_P_H
