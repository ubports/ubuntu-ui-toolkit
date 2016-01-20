#include "ucmainviewbase.h"
#include "ucmainviewbase_p.h"

#include "ucactionmanager.h"
#include "ucactioncontext.h"
#include "ucapplication.h"
#include "uctheme.h"
#include "i18n.h"

#include <QCoreApplication>

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

QQmlListProperty<UCAction> UCMainViewBase::actions() const
{
    return d_func()->m_actionManager->actions();
}

UCActionManager *UCMainViewBase::actionManager() const
{
    return d_func()->m_actionManager;
}

UCPopupContext *UCMainViewBase::actionContext() const
{
    return d_func()->m_actionContext;
}
