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

#include "ucmainviewbase.h"
#include "ucmainviewbase_p.h"

#include "ucactionmanager.h"
#include "ucactioncontext.h"
#include "ucapplication.h"
#include "uctheme.h"
#include "i18n.h"

#include <QCoreApplication>

/*!
  \internal
  \qmlabstract MainViewBase
  \inqmlmodule Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief The base class for MainView and MultiColumnView.
*/
UCMainViewBasePrivate::UCMainViewBasePrivate()
    : m_actionManager(nullptr),
      m_actionContext(nullptr),
      m_anchorToKeyboard(false)
{
}

void UCMainViewBasePrivate::init()
{
    Q_Q(UCMainViewBase);

    //need to init here because the q pointer is null in constructor
    m_actionManager = new UCActionManager(q);
    m_actionContext = new UCPopupContext(q);

    m_actionContext->setObjectName(QStringLiteral("RootContext"));
    m_actionContext->setActive(true);
    q->setActive(true);

    QObject::connect(m_actionManager,&UCActionManager::quit, [](){
        // FIXME Wire this up to the application lifecycle management API instead of quit().
        qApp->quit();
    });
}

void UCMainViewBasePrivate::_q_headerColorBinding(const QColor &col)
{
    Q_Q(UCMainViewBase);

    if (m_headerColor == col)
        return;

    m_headerColor = col;
    Q_EMIT q->headerColorChanged(col);
}

void UCMainViewBasePrivate::_q_footerColorBinding(const QColor &col)
{
    Q_Q(UCMainViewBase);

    if (m_footerColor == col)
        return;

    m_footerColor = col;
    Q_EMIT q->footerColorChanged(col);
}

UCMainViewBase::UCMainViewBase(QQuickItem *parent)
    : UCPageTreeNode(*(new UCMainViewBasePrivate), parent)
{
    d_func()->init();
}

UCMainViewBase::UCMainViewBase(UCMainViewBasePrivate &dd, QQuickItem *parent)
    : UCPageTreeNode( dd, parent)
{
    d_func()->init();
}

/*!
  \qmlproperty string MainViewBase::applicationName
  The property holds the application's name, which must be the same as the
  desktop file's name.
  The name also sets the name of the QCoreApplication and defaults for data
  and cache folders that work on the desktop and under confinement, as well as
  the default gettext domain.
  C++ code that writes files may use QStandardPaths::writableLocation with
  QStandardPaths::DataLocation or QStandardPaths::CacheLocation.
*/
QString UCMainViewBase::applicationName() const
{
    return d_func()->m_applicationName;
}

void UCMainViewBase::setApplicationName(QString applicationName)
{
    Q_D(UCMainViewBase);
    if (d->m_applicationName == applicationName)
        return;

    d->m_applicationName = applicationName;

    if (applicationName != "") {
        UbuntuI18n::instance().setDomain(applicationName);
        UCApplication::instance().setApplicationName(applicationName);
    }
    Q_EMIT applicationNameChanged(applicationName);
}

/*!
  \qmlproperty bool MainViewBase::anchorToKeyboard

  The property holds if the application should automatically resize the
  contents when the input method appears

  The default value is false.
*/
bool UCMainViewBase::anchorToKeyboard() const
{
    return d_func()->m_anchorToKeyboard;
}

void UCMainViewBase::setAnchorToKeyboard(bool anchorToKeyboard)
{
    Q_D(UCMainViewBase);
    if (d->m_anchorToKeyboard == anchorToKeyboard)
        return;

    d->m_anchorToKeyboard = anchorToKeyboard;
    Q_EMIT anchorToKeyboardChanged(anchorToKeyboard);
}

/*!
  \qmlproperty color MainView::headerColor
  Color of the header's background.

  \sa backgroundColor, footerColor
*/
QColor UCMainViewBase::headerColor() const
{
    return d_func()->m_headerColor;
}

void UCMainViewBase::setHeaderColor(QColor headerColor)
{
    Q_D(UCMainViewBase);

    //disable binding to background color
    d->m_flags |= UCMainViewBasePrivate::CustomHeaderColor;
    d->_q_headerColorBinding(headerColor);
}

/*!
  \qmlproperty color MainView::backgroundColor
  Color of the background.

  The background is usually a single color. However if \l headerColor
  or \l footerColor are set then a gradient of colors will be drawn.

  For example, in order for the MainView to draw a color gradient beneath
  the content:
  \qml
      import QtQuick 2.4
      import Ubuntu.Components 1.2

      MainView {
          width: units.gu(40)
          height: units.gu(60)

          headerColor: "#343C60"
          backgroundColor: "#6A69A2"
          footerColor: "#8896D5"
      }
  \endqml

  \sa footerColor, headerColor
*/
QColor UCMainViewBase::backgroundColor() const
{
    return d_func()->m_backgroundColor;
}

void UCMainViewBase::setBackgroundColor(QColor backgroundColor)
{
    Q_D(UCMainViewBase);

    if (d->m_backgroundColor == backgroundColor)
        return;

    d->m_backgroundColor = backgroundColor;

    if (!(d->m_flags & UCMainViewBasePrivate::CustomHeaderColor))
        d->_q_headerColorBinding(d->m_backgroundColor);
    if (!(d->m_flags & UCMainViewBasePrivate::CustomFooterColor))
        d->_q_footerColorBinding(d->m_backgroundColor);

    // FIXME: Define the background colors in MainViewStyle and get rid of the properties
    //  in MainViewBase. That removes the need for auto-theming.

    /*
      As we don't know the order the property bindings and onXXXChanged signals are evaluated
      we should rely only on one property when changing the theme to avoid intermediate
      theme changes due to properties being evaluated separately.

      Qt bug: https://bugreports.qt-project.org/browse/QTBUG-11712
     */
    if (d->m_backgroundColor != getTheme()->getPaletteColor("normal", "background")) {
        QString themeName = d->m_backgroundColor.lightnessF() >= 0.85 ? QStringLiteral("Ambiance")
                                                                   : QStringLiteral("SuruDark");

        // only change the theme if the current one is a system one.
        if (getTheme()->name().startsWith(QStringLiteral("Ubuntu.Components.Themes"))) {
            getTheme()->setName(QString(QStringLiteral("Ubuntu.Components.Themes.%1")).arg(themeName));
        }
    }

    Q_EMIT backgroundColorChanged(backgroundColor);
}

/*!
  \qmlproperty color MainView::footerColor
  Color of the footer's background.

  \sa backgroundColor, headerColor
*/
QColor UCMainViewBase::footerColor() const
{
    return d_func()->m_footerColor;
}

void UCMainViewBase::setFooterColor(QColor footerColor)
{
    Q_D(UCMainViewBase);

    //disable binding to background color
    d->m_flags |= UCMainViewBasePrivate::CustomFooterColor;
    d->_q_footerColorBinding(footerColor);
}

/*!
  \qmlproperty list<Action> MainViewBase::actions
  \readonly

  A global list of actions that will be available to the system (including HUD)
  as long as the application is running. For actions that are not always available to the
  system, but only when a certain \l Page is active, see the actions property of \l Page.
*/
QQmlListProperty<UCAction> UCMainViewBase::actions() const
{
    return d_func()->m_actionManager->actions();
}

/*!
  \qmlproperty ActionManager MainView::actionManager
  \readonly

  The ActionManager that supervises the global and local ActionContexts.
  The \l actions property should be used preferably since it covers most
  use cases. The ActionManager is accessible to have a more refined control
  over the actions, e.g. if one wants to add/remove actions dynamically, create
  specific action contexts, etc.
*/
UCActionManager *UCMainViewBase::actionManager() const
{
    return d_func()->m_actionManager;
}

/*!
  \qmlproperty ActrionContext MainView::actionContext
  \readonly
  \since Ubuntu.Components 1.3
  The action context of the MainView.
  */
UCPopupContext *UCMainViewBase::actionContext() const
{
    return d_func()->m_actionContext;
}

void UCMainViewBase::componentComplete()
{
    UCPageTreeNode::componentComplete();
    d_func()->setStyleName(QStringLiteral("MainViewStyle"));
}
