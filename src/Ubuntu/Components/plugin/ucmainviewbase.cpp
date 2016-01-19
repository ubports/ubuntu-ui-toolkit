#include "ucmainviewbase.h"
#include "ucmainviewbase_p.h"

#include "ucactionmanager.h"
#include "ucapplication.h"
#include "i18n.h"

#include <QCoreApplication>

UCMainViewBasePrivate::UCMainViewBasePrivate()
    : m_anchorToKeyboard(false)
{
    Q_Q(UCMainViewBase);
    m_actionManager = new UCActionManager(q);

    QObject::connect(m_actionManager,&UCActionManager::quit, [](){
        // FIXME Wire this up to the application lifecycle management API instead of quit().
        qApp->quit();
    });
}

void UCMainViewBasePrivate::init()
{
    Q_Q(UCMainViewBase);
    q->setActive(true);
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
    if (d->m_headerColor == headerColor)
        return;

    d->m_headerColor = headerColor;
    Q_EMIT headerColorChanged(headerColor);
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
    Q_EMIT backgroundColorChanged(backgroundColor);
}

QColor UCMainViewBase::footerColor() const
{
    return d_func()->m_footerColor;
}

void UCMainViewBase::setFooterColor(QColor footerColor)
{
    Q_D(UCMainViewBase);
    if (d->m_footerColor == footerColor)
        return;

    d->m_footerColor = footerColor;
    Q_EMIT footerColorChanged(footerColor);
}

QQmlListProperty<UCAction> UCMainViewBase::actions() const
{
    return d_func()->m_actionManager->actions();
}

UCActionManager *UCMainViewBase::actionManager() const
{
    return d_func()->m_actionManager;
}
