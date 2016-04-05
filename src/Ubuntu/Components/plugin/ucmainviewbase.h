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

#ifndef UCMAINVIEWBASE_H
#define UCMAINVIEWBASE_H

#include "ucpagetreenode.h"

class UCMainViewBasePrivate;
class UCActionManager;
class UCPopupContext;
class UCAction;

class UCMainViewBase : public UCPageTreeNode
{
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(bool anchorToKeyboard READ anchorToKeyboard WRITE setAnchorToKeyboard NOTIFY anchorToKeyboardChanged)
    Q_PROPERTY(QColor headerColor READ headerColor WRITE setHeaderColor NOTIFY headerColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor footerColor READ footerColor WRITE setFooterColor NOTIFY footerColorChanged)
    Q_PROPERTY(QQmlListProperty<UCAction> actions READ actions)
    Q_PROPERTY(UCActionManager* actionManager READ actionManager NOTIFY actionManagerChanged)
    Q_PROPERTY(UCPopupContext* actionContext READ actionContext NOTIFY actionContextChanged)

public:
    UCMainViewBase(QQuickItem *parent = nullptr);


    QString applicationName() const;
    void setApplicationName(QString applicationName);

    bool anchorToKeyboard() const;
    void setAnchorToKeyboard(bool anchorToKeyboard);

    QColor headerColor() const;
    void setHeaderColor(QColor headerColor);

    QColor backgroundColor() const;
    void setBackgroundColor(QColor backgroundColor);

    QColor footerColor() const;
    void setFooterColor(QColor footerColor);

    QQmlListProperty<UCAction> actions() const;

    UCActionManager *actionManager() const;

    UCPopupContext* actionContext() const;

    // QQmlParserStatus interface
    virtual void componentComplete() override;
    virtual void classBegin() override;

Q_SIGNALS:
    void applicationNameChanged(QString applicationName);
    void anchorToKeyboardChanged(bool anchorToKeyboard);
    void headerColorChanged(const QColor &headerColor);
    void backgroundColorChanged(const QColor &backgroundColor);
    void footerColorChanged(const QColor &footerColor);
    void actionManagerChanged(UCActionManager* actionManager);
    void actionContextChanged(UCPopupContext* actionContext);

protected:
    UCMainViewBase(UCMainViewBasePrivate &dd, QQuickItem *parent);

private:
    Q_DECLARE_PRIVATE(UCMainViewBase)
    Q_PRIVATE_SLOT(d_func(), void _q_updateWindow())
};

#endif // UCMAINVIEWBASE_H
