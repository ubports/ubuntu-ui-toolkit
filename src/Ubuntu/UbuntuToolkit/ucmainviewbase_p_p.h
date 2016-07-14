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

#ifndef UCMAINVIEWBASE_P_H
#define UCMAINVIEWBASE_P_H

#include "ucpagetreenode_p_p.h"
#include "ucmainviewbase_p.h"
#include <QQmlProperty>

UT_NAMESPACE_BEGIN

class UCMainViewBase;
class UCActionManager;
class UCPopupContext;

class UCMainViewBasePrivate : public UCPageTreeNodePrivate
{
    Q_DECLARE_PUBLIC(UCMainViewBase)

public:
    UCMainViewBasePrivate();
    void init();

    enum PropertyFlags {
        CustomHeaderColor     = 0x01,
        CustomBackgroundColor = 0x02,
        CustomFooterColor     = 0x04
    };

    void _q_headerColorBinding (const QColor &col);
    void _q_footerColorBinding (const QColor &col);
    void doAutoTheme();
    void _q_updateWindow();

    QString m_applicationName;
    QColor m_headerColor;
    QColor m_backgroundColor;
    QColor m_footerColor;
    UCActionManager *m_actionManager = nullptr;
    UCPopupContext* m_actionContext = nullptr;
    qint8 m_flags;
    bool m_anchorToKeyboard:1;

};

UT_NAMESPACE_END

#endif // UCMAINVIEWBASE_P_H
