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

#ifndef UCSTYLEDITEMBASE_P_H
#define UCSTYLEDITEMBASE_P_H

#include <QtQuick/private/qquickitem_p.h>
#include "ucstyleditembase.h"
#include "ucthemingextension.h"
#include "ucimportversionchecker_p.h"

class QQuickMouseArea;

namespace UbuntuToolkit {

class UCStyledItemBase;
class UBUNTUTOOLKIT_EXPORT UCStyledItemBasePrivate : public QQuickItemPrivate, public UCImportVersionChecker
{
    Q_INTERFACES(UT_PREPEND_NAMESPACE(UCThemingExtension))
    Q_DECLARE_PUBLIC(UCStyledItemBase)
public:

    static UCStyledItemBasePrivate *get(UCStyledItemBase *item) {
        return item->d_func();
    }

    void _q_styleResized();

    UCStyledItemBasePrivate();
    virtual ~UCStyledItemBasePrivate();
    void init();

    virtual void setFocusable(bool focus);
    bool isParentFocusable();

    QQmlComponent *style() const;
    void setStyle(QQmlComponent *style);
    void resetStyle();
    QQuickItem *styleInstance();

    QString styleName() const;
    void setStyleName(const QString &name);

    virtual void preStyleChanged();
    virtual void postStyleChanged() {}
    virtual bool loadStyleItem(bool animated = true);
    virtual void completeComponentInitialization();

    // from UCImportVersionChecker
    QString propertyForVersion(quint16 version) const override;

public:

    QPointer<QQmlContext> styleItemContext;
    QString styleDocument;
    QQuickItem *oldParentItem;
    QQmlComponent *styleComponent;
    QQuickItem *styleItem;
    quint16 styleVersion;
    bool keyNavigationFocus:1;
    bool activeFocusOnPress:1;
    bool wasStyleLoaded:1;
    bool isFocusScope:1;

protected:

    void connectStyleSizeChanges(bool attach);
};

UT_NAMESPACE_END

#endif // UCSTYLEDITEMBASE_P_H
