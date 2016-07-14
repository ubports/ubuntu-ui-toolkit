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

#include "ucpagetreenode_p.h"

UT_NAMESPACE_BEGIN

class UCMainViewBasePrivate;
class UCActionManager;
class UCPopupContext;
class UCAction;

class UBUNTUTOOLKIT_EXPORT UCMainViewBase : public UCPageTreeNode
{
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(bool anchorToKeyboard READ anchorToKeyboard WRITE setAnchorToKeyboard NOTIFY anchorToKeyboardChanged)
    Q_PROPERTY(QColor headerColor READ headerColor WRITE setHeaderColor NOTIFY headerColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor footerColor READ footerColor WRITE setFooterColor NOTIFY footerColorChanged)
#ifndef Q_QDOC
    Q_PROPERTY(QQmlListProperty<UT_PREPEND_NAMESPACE(UCAction)> actions READ actions)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCActionManager)* actionManager READ actionManager NOTIFY actionManagerChanged)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCPopupContext)* actionContext READ actionContext NOTIFY actionContextChanged)
#else
    Q_PROPERTY(QQmlListProperty<UCAction> actions READ actions)
    Q_PROPERTY(UCActionManager* actionManager READ actionManager NOTIFY actionManagerChanged)
    Q_PROPERTY(UCPopupContext* actionContext READ actionContext NOTIFY actionContextChanged)
#endif

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
    void componentComplete() override;
    void classBegin() override;

Q_SIGNALS:
    void applicationNameChanged(QString applicationName);
    void anchorToKeyboardChanged(bool anchorToKeyboard);
    void headerColorChanged(const QColor &headerColor);
    void backgroundColorChanged(const QColor &backgroundColor);
    void footerColorChanged(const QColor &footerColor);
#ifndef Q_QDOC
    void actionManagerChanged(UT_PREPEND_NAMESPACE(UCActionManager)* actionManager);
    void actionContextChanged(UT_PREPEND_NAMESPACE(UCPopupContext)* actionContext);
#else
    void actionManagerChanged(UCActionManager* actionManager);
    void actionContextChanged(UCPopupContext* actionContext);
#endif

protected:
    UCMainViewBase(UCMainViewBasePrivate &dd, QQuickItem *parent);

private:
    Q_DECLARE_PRIVATE(UCMainViewBase)
    Q_PRIVATE_SLOT(d_func(), void _q_updateWindow())
};

UT_NAMESPACE_END

#endif // UCMAINVIEWBASE_H
