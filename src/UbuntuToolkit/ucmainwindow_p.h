/*
 * Copyright 2016-2017 Canonical Ltd.
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

#ifndef UCMAINWINDOW_P_H
#define UCMAINWINDOW_P_H

#include <QtQuick/QQuickWindow>

#include <UbuntuToolkit/private/i18n_p.h>
#include <UbuntuToolkit/private/ucunits_p.h>

UT_NAMESPACE_BEGIN

class UCMainWindowPrivate;
class UCPopupContext;
class UCAction;

class UBUNTUTOOLKIT_EXPORT UCMainWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(QString organizationName READ organizationName WRITE setOrganizationName NOTIFY organizationNameChanged)
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCUnits)* units READ units NOTIFY unitsChanged)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UbuntuI18n)* i18n READ i18n NOTIFY i18nChanged)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCPopupContext)* actionContext READ actionContext NOTIFY actionContextChanged)
    Q_PROPERTY(UT_PREPEND_NAMESPACE(QQuickItem)* visualRoot READ visualRoot WRITE setVisualRoot NOTIFY visualRootChanged)
#else
    Q_PROPERTY(UCUnits* units READ units NOTIFY unitsChanged)
    Q_PROPERTY(UbuntuI18n* i18n READ i18n NOTIFY i18nChanged)
    Q_PROPERTY(UCPopupContext* actionContext READ actionContext NOTIFY actionContextChanged)
    Q_PROPERTY(QQuickItem* visualRoot READ visualRoot WRITE setVisualRoot NOTIFY visualRootChanged)
#endif

public:
    UCMainWindow(QWindow *parent = nullptr);


    QString applicationName() const;
    void setApplicationName(QString applicationName);
    QString organizationName() const;
    void setOrganizationName(QString organizationName);

    UCUnits* units();
    UbuntuI18n* i18n() const;

    UCPopupContext* actionContext() const;

    QQuickItem* visualRoot() const;
    void setVisualRoot(QQuickItem*);

Q_SIGNALS:
    void applicationNameChanged(QString applicationName);
    void organizationNameChanged(QString applicationName);
    void i18nChanged();
    void unitsChanged();
#ifndef Q_QDOC
    void actionContextChanged(UT_PREPEND_NAMESPACE(UCPopupContext)* actionContext);
    void visualRootChanged(UT_PREPEND_NAMESPACE(QQuickItem)* visualRoot);
#else
    void actionContextChanged(UCPopupContext* actionContext);
    void visualRootChanged(QQuickItem* visualRoot);
#endif

private:
    Q_DECLARE_PRIVATE(UCMainWindow)
};

UT_NAMESPACE_END

#endif // UCMAINWINDOW_P_H
