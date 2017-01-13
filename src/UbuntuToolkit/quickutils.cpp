/*
 * Copyright 2012 Canonical Ltd.
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

#include "quickutils_p.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QAbstractProxyModel>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlPropertyMap>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtQuick/private/qquicktextinput_p.h>
#include <QtQuick/private/qquicktextedit_p.h>
#include <QtSystemInfo/QInputInfoManager>

UT_NAMESPACE_BEGIN

QuickUtils *QuickUtils::m_instance = nullptr;

QuickUtils::QuickUtils(QObject *parent) :
    QObject(parent),
    m_rootWindow(0),
    m_rootView(0),
    m_mouseAttached(false),
    m_keyboardAttached(false),
    m_explicitMouseAttached(false),
    m_explicitKeyboardAttached(false)
{
    QGuiApplication::instance()->installEventFilter(this);
    m_omitIM << QStringLiteral("ibus") << QStringLiteral("none") << QStringLiteral("compose");

    m_inputInfo = new QInputInfoManager(this);
    connect(m_inputInfo, &QInputInfoManager::ready,
            this, &QuickUtils::onInputInfoReady);
    // the default values of mouse and keyboard attachment depend on the input info
    connect(m_inputInfo, &QInputInfoManager::deviceAdded,
            this, &QuickUtils::onDeviceAdded);
    connect(m_inputInfo, &QInputInfoManager::deviceRemoved,
            this, &QuickUtils::onDeviceRemoved);
}

void QuickUtils::onInputInfoReady()
{
    QMapIterator<QString, QInputDevice*> i(m_inputInfo->deviceMap());
    while (i.hasNext()) {
        i.next();
        registerDevice(i.value(), i.key());
    }

    m_inputInfo->setFilter(QInputDevice::Mouse | QInputDevice::TouchPad | QInputDevice::Keyboard);
}

void QuickUtils::onDeviceAdded(QInputDevice *device)
{
    QMapIterator<QString, QInputDevice*> i(m_inputInfo->deviceMap());
    while (i.hasNext()) {
        i.next();
        if (i.value() == device) {
            registerDevice(device, i.key());
            break;
        }
    }
}

void QuickUtils::onDeviceRemoved(const QString deviceId)
{
    // the device info is removed by now, so we must look at the internal cache
    if (m_keyboards.remove(deviceId)) {
        if (!m_explicitKeyboardAttached && !m_keyboards.size()) {
            m_keyboardAttached = false;
            Q_EMIT keyboardAttachedChanged();
        }
    }
    if (m_mice.remove(deviceId)) {
        if (!m_explicitMouseAttached && !m_mice.size()) {
            m_mouseAttached = false;
            Q_EMIT mouseAttachedChanged();
        }
    }
}

void QuickUtils::registerDevice(QInputDevice *device, const QString &deviceId)
{
    if (device->types().testFlag(QInputDevice::Keyboard)) {
        m_keyboards.insert(deviceId);
        if (!m_explicitKeyboardAttached && !m_keyboardAttached) {
            m_keyboardAttached = true;
            Q_EMIT keyboardAttachedChanged();
        }
    }
    if (device->types().testFlag(QInputDevice::Mouse)
            || device->types().testFlag(QInputDevice::TouchPad)) {
        m_mice.insert(deviceId);
        if (!m_explicitMouseAttached && !m_mouseAttached) {
            m_mouseAttached = true;
            Q_EMIT mouseAttachedChanged();
        }
    }
}

void QuickUtils::setMouseAttached(bool set)
{
    m_explicitMouseAttached = true;
    if (set == m_mouseAttached) {
        return;
    }
    m_mouseAttached = set;
    Q_EMIT mouseAttachedChanged();
}

void QuickUtils::setKeyboardAttached(bool set)
{
    m_explicitKeyboardAttached = true;
    if (set == m_keyboardAttached) {
        return;
    }
    m_keyboardAttached = true;
    Q_EMIT keyboardAttachedChanged();
}

/*!
 * \internal
 * Filter events to catch ChildAdded, when the the application gets the topmost
 * window assigned. Need to check the topmost windows each time as widgets added
 * to the application are not signaled in any other way.
 */
bool QuickUtils::eventFilter(QObject *obj, QEvent *event)
{
    if (!m_rootView && (event->type() == QEvent::ApplicationActivate)) {
        lookupQuickView();
        Q_EMIT activated();
    }
    if (event->type() == QEvent::ApplicationDeactivate) {
        Q_EMIT deactivated();
    }

    return QObject::eventFilter(obj, event);
}

/*!
 * \internal
 * \deprecated
 * Returns the current root object.
 */
QQuickItem *QuickUtils::rootObject()
{
    qmlInfo(this) << "WARNING: QuickUtils.rootObject property is deprecated: Use QuickUtils::rootItem() function instead.";
    if (!m_rootView)
        lookupQuickView();
    return (m_rootView) ? m_rootView->rootObject() : 0;
}

/*!
 * \internal
 * Returns the root item of a given item. In case there is a QQuickWindow (Window)
 * found in the hierarchy, the function will return the contentItem of the window.
 */
QQuickItem *QuickUtils::rootItem(QObject *object)
{
    // make sure we have the m_rootView updated
    lookupQuickView();

    UCMainWindow *mainWindow(qobject_cast(<UCMainWindow*>(m_rootWindow)));
    if (mainWindow && mainWindow->visualRoot()) {
        return mainWindow->visualRoot();
    }

    if (!object) {
        return m_rootView ? m_rootView->rootObject() : (m_rootWindow ? m_rootWindow->contentItem() : 0);
    }

    QQuickItem *item = qobject_cast<QQuickItem*>(object);
    // the given object may be a non-visual element (QtObject or QQmlComponent)
    // therefore those objects' parent object should be considered
    QQuickItem *parentItem = item ? item->parentItem() : qobject_cast<QQuickItem*>(object->parent());
    while (parentItem && parentItem->parentItem()) {
        parentItem = parentItem->parentItem();
    }

    if (m_rootView && (m_rootView->contentItem() == parentItem)) {
        // when traversing visual parents of an element from the application,
        // we reach QQuickView's contentItem, whose size is invalid. Therefore
        // we need to return the QQuickView's rootObject() instead of the topmost
        // item found
        parentItem = m_rootView->rootObject();
    }
    // in case the item found is derived from internal QQuickRootItem, return its first child
    if (parentItem && parentItem->inherits("QQuickRootItem")) {
        parentItem = parentItem->childItems()[0];
    }
    return parentItem;
}


QString QuickUtils::inputMethodProvider() const
{
    QString im = QString::fromLocal8Bit(getenv("QT_IM_MODULE"));

    return m_omitIM.contains(im) ? QString() : im;
}

bool QuickUtils::touchScreenAvailable() const
{
    // publish internal context property to detect whether we have touch device or not
    QList<const QTouchDevice*> touchDevices = QTouchDevice::devices();
    Q_FOREACH(const QTouchDevice *device, touchDevices) {
        if (device->type() == QTouchDevice::TouchScreen) {
            return true;
        }
    }
    return false;
}

/*!
 * \internal
 * Returns the class name (type) of a QtQuick item.
 */
QString QuickUtils::className(QObject *item)
{
    if (!item) {
        return QStringLiteral("(null)");
    }
    QString result = QString::fromLatin1(item->metaObject()->className());
    return result.left(result.indexOf(QStringLiteral("_QML")));
}

/*!
 * \internal
 * The function checks whether an item inherits a given class name.
 */
bool QuickUtils::inherits(QObject *object, const QString &fromClass)
{
    if (!object || fromClass.isEmpty()) {
        return false;
    }
    const QMetaObject *mo = object->metaObject();
    QString className;
    while (mo) {
        className = QString::fromLatin1(mo->className());
        className = className.left(className.indexOf(QStringLiteral("_QML")));
        if (className == fromClass) {
            return true;
        }
        mo = mo->superClass();
    }
    return false;
}



/*!
 * \internal
 * Get QQuickView from the application's window list and connect its status change
 * signal as the root element is set after the root element completion.
 */
void QuickUtils::lookupQuickView()
{
    if (m_rootWindow)
        return;
    Q_FOREACH (QWindow *w, QGuiApplication::topLevelWindows()) {
        m_rootWindow = qobject_cast<QQuickWindow*>(w);
        m_rootView = qobject_cast<QQuickView*>(w);
        if (m_rootView) {
            // connect in case we get the root object changed
            QObject::connect(m_rootView, SIGNAL(statusChanged(QQuickView::Status)),
                             this, SIGNAL(rootObjectChanged()));
            // emit changed signal so we update the eventual bindings
            if (m_rootView->rootObject())
                Q_EMIT rootObjectChanged();
            break;
        }
    }
}

QObject* QuickUtils::createQmlObject(const QUrl &url, QQmlEngine *engine)
{
    /* FIXME: if the directory pointed to by url contains a qmldir file that
       declares a JavaScript module then QQmlComponent::create() fails with
       the error "QQmlComponent: Component is not ready".
    */
    QQmlComponent *component = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous);
    QObject* result(Q_NULLPTR);
    if (component->isError()) {
        qmlInfo(engine) << component->errorString();
    } else {
        result = component->create();
    }
    delete component;
    return result;
}

bool QuickUtils::showDeprecationWarnings() {
    static int showWarnings = 0;
    if (showWarnings == 0) {
        QByteArray warningsFlag = qgetenv("SUPPRESS_DEPRECATED_NOTE");
        showWarnings = (warningsFlag.isEmpty() || warningsFlag == "yes") ? 1 : 2;
    }
    return showWarnings == 2;
}

// check whether an item is a descendant of parent
bool QuickUtils::descendantItemOf(QQuickItem *item, const QQuickItem *parent)
{
    while (item && parent) {
        if (item == parent) {
            return true;
        }
        item = item->parentItem();
    }
    return false;
}

// returns the first key-focusable child item
QQuickItem *QuickUtils::firstFocusableChild(QQuickItem *item)
{
    if (!item) {
        return Q_NULLPTR;
    }
    const QList<QQuickItem*> &list = item->childItems();
    for (int i = 0; i < list.count(); i++) {
        QQuickItem *child = list.at(i);
        if (child->activeFocusOnTab()) {
            return child;
        }
        QQuickItem *focus = firstFocusableChild(child);
        if (focus) {
            return focus;
        }
    }
    return Q_NULLPTR;
}

// returns the last key-focusable child item
QQuickItem *QuickUtils::lastFocusableChild(QQuickItem *item)
{
    if (!item) {
        return Q_NULLPTR;
    }
    const QList<QQuickItem*> &list = item->childItems();
    int i = list.count() - 1;
    while (i >= 0) {
        QQuickItem *child = list.at(i--);
        if (child->activeFocusOnTab()) {
            return child;
        }
        QQuickItem *focus = lastFocusableChild(child);
        if (focus) {
            return focus;
        }
    }
    return Q_NULLPTR;
}

UT_NAMESPACE_END
