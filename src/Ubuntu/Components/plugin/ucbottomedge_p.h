/*
 * Copyright 2015 Canonical Ltd.
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

#ifndef UCBOTTOMEDGE_P_H
#define UCBOTTOMEDGE_P_H

#include "ucbottomedge.h"
#include "ucstyleditembase_p.h"

class UCBottomEdgeStyle;
class UCBottomEdgePrivate : public UCStyledItemBasePrivate, protected QQuickItemChangeListener
{
    Q_DECLARE_PUBLIC(UCBottomEdge)

public:
    UCBottomEdgePrivate();

    static UCBottomEdgePrivate *get(UCBottomEdge *item)
    {
        return item->d_func();
    }
    void init();

    void createDefaultSections();
    void updateProgressionStates();
    // panel positioning
    void positionPanel(qreal position);
    // internal setters
    void setState(UCBottomEdge::State state);

    // from UCStyledItemBase
    virtual bool loadStyleItem(bool animated = true);
    // from QQuickItemChangeListener
    void itemChildAdded(QQuickItem *item, QQuickItem *child);
    void itemChildRemoved(QQuickItem *item, QQuickItem *child);


    // members
    QList<UCBottomEdgeSection*> sections;
    QPointer<UCBottomEdgeSection> lastSection;
    QUrl contentUrl;
    QQuickItem *hint;
    QQmlComponent *contentComponent;
    UCBottomEdgeStyle *bottomPanel;

    qreal commitPoint;
    UCBottomEdge::State state;

    bool defaultSectionsReset:1;
    bool blockSectionProgressCheck:1;

};

#endif // UCBOTTOMEDGE_P_H
