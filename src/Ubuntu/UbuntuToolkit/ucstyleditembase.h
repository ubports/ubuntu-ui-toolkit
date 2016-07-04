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

#ifndef UCSTYLEDITEMBASE_H
#define UCSTYLEDITEMBASE_H

#include <QtQuick/QQuickItem>
#include "ucthemingextension.h"
#include "ubuntutoolkitglobal.h"

namespace UbuntuToolkit {

class UCStyledItemBasePrivate;
class UCTheme;
class UCStyleHints;
class UBUNTUTOOLKIT_EXPORT UCStyledItemBase : public QQuickItem, public UCThemingExtension
{
    Q_OBJECT
    Q_INTERFACES(UT_PREPEND_NAMESPACE(UCThemingExtension))
    Q_PROPERTY(bool keyNavigationFocus
              READ keyNavigationFocus
              NOTIFY keyNavigationFocusChanged REVISION 2)
    Q_PROPERTY(bool activeFocusOnPress
               READ activefocusOnPress WRITE setActiveFocusOnPress
               NOTIFY activeFocusOnPressChanged REVISION 1)
    // FIXME Re-expose property that would be inaccessible due to a QML bug
    // https://bugs.launchpad.net/ubuntu/+source/qtdeclarative-opensource-src/+bug/1389721
    Q_PROPERTY(bool activeFocusOnTab
            READ activeFocusOnTab2
            WRITE setActiveFocusOnTab2
            NOTIFY activeFocusOnTabChanged2 FINAL)
    Q_PRIVATE_PROPERTY(UCStyledItemBase::d_func(), QQmlComponent *style READ style WRITE setStyle RESET resetStyle NOTIFY styleChanged FINAL DESIGNABLE false)
    Q_PRIVATE_PROPERTY(UCStyledItemBase::d_func(), QQuickItem *__styleInstance READ styleInstance NOTIFY styleInstanceChanged FINAL DESIGNABLE false)
    Q_PRIVATE_PROPERTY(UCStyledItemBase::d_func(), QString styleName READ styleName WRITE setStyleName NOTIFY styleNameChanged FINAL REVISION 2)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCTheme) *theme READ getTheme WRITE setTheme RESET resetTheme NOTIFY themeChanged FINAL REVISION 2)
public:
    explicit UCStyledItemBase(QQuickItem *parent = 0);

    virtual bool keyNavigationFocus() const;
    bool activefocusOnPress() const;
    void setActiveFocusOnPress(bool value);
    bool activeFocusOnTab2() const;
    void setActiveFocusOnTab2(bool value);

public Q_SLOTS:
    Q_REVISION(1) bool requestFocus(Qt::FocusReason reason = Qt::OtherFocusReason);

Q_SIGNALS:
    void styleChanged();
    void styleInstanceChanged();
    Q_REVISION(2) void keyNavigationFocusChanged();
    Q_REVISION(1) void activeFocusOnPressChanged();
    Q_REVISION(1) void activeFocusOnTabChanged2();
    Q_REVISION(2) void themeChanged();
    Q_REVISION(2) void styleNameChanged();

protected:
    UCStyledItemBase(UCStyledItemBasePrivate &, QQuickItem *parent);

    // from UCThemingExtension interface
    void preThemeChanged() override;
    void postThemeChanged() override;

    void classBegin() override;
    void componentComplete() override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void focusInEvent(QFocusEvent *key) override;
    void setKeyNavigationFocus(bool value);
    void mousePressEvent(QMouseEvent *event) override;
    bool childMouseEventFilter(QQuickItem *child, QEvent *event) override;

private:
    Q_DECLARE_PRIVATE(UCStyledItemBase)
    Q_PRIVATE_SLOT(d_func(), void _q_styleResized())
};

UT_NAMESPACE_END

#endif // UCSTYLEDITEMBASE_H
