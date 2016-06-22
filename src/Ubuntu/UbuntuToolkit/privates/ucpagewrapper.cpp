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

#include "ucpagewrapper.h"
#include "ucpagewrapper_p.h"
#include "ucpagewrapperincubator_p.h"

#include <QQmlEngine>
#include <QQmlContext>

UT_NAMESPACE_BEGIN

UCPageWrapperPrivate::UCPageWrapperPrivate() :
    m_object(nullptr),
    m_parentPage(nullptr),
    m_parentWrapper(nullptr),
    m_pageHolder(nullptr),
    m_incubator(nullptr),
    m_component(nullptr),
    m_itemContext(nullptr),
    m_state(Waiting),
    m_column(0),
    m_canDestroy(false),
    m_synchronous(true),
    m_ownsComponent(false)
{ }

UCPageWrapperPrivate::~UCPageWrapperPrivate()
{
    reset();
}

void UCPageWrapperPrivate::init()
{
    Q_Q(UCPageWrapper);

    //This value is updated when a PageWrapper is pushed to/popped from a PageStack.
    q->setActive(false);

    q->setVisible(false);

    //bind the value of visible to active
    QObject::connect(q, SIGNAL(activeChanged(bool)),
                     q, SLOT(onActiveChanged()));


    QObject::connect(q, &UCPageWrapper::activeChanged,
                     [this](){
        if (m_reference.isValid()) {
            if (m_active) {
                this->activate();
            } else {
                this->deactivate();
            }
        }
    });

    //FIXME bind re-exposed property signals lp:1389721
    QObject::connect(q, SIGNAL(themeChanged()), q, SIGNAL(themeChanged2()));
}

/*!
 Starts loading the component in m_reference, cleans up old objects
 */
void UCPageWrapperPrivate::initPage()
{
    //make sure we are clean
    reset();
    nextStep();
}

/*!
 Reset the internal state of the PageWrapper, deletes also all objects
 that were created by this PageWrapper instance
 */
void UCPageWrapperPrivate::reset()
{
    Q_Q(UCPageWrapper);

    if (m_incubator) {
        //if incubator is READY the object() is not deleted
        if (m_incubator->status() == QQmlIncubator::Ready && m_incubator->object()) {
            m_incubator->object()->deleteLater();
        }
        m_incubator->clear();
        destroyIncubator();
    }

    if (m_object) {
        if (m_canDestroy) {
            m_object->deleteLater();
        }
        q->setObject(nullptr);
    }

    if (m_component && m_ownsComponent) {
        delete m_component;
        m_component = nullptr;
    }

    //if itemContext is a valid pointer at this place
    //it was never used and can be deleted
    if (m_itemContext) {
        //use deleteLater just to be sure
        m_itemContext->deleteLater();
        m_itemContext = nullptr;
    }

    m_state = Waiting;
}

/*!
  \internal
  Create the page object if needed, and make the page object visible.
 */
void UCPageWrapperPrivate::activate()
{
    Q_Q(UCPageWrapper);
    if (!m_object
            && m_state != LoadingComponent
            && m_state != CreatingObject)
    {
        initPage();
    }

    //make sure object is valid now
    if (!m_object)
        return;

    QQuickItem *item = qobject_cast<QQuickItem *>(m_object);
    if (item){
        // Having the same page pushed multiple times on a stack changes
        // the parent of the page object. Change it back here.
        item->setParentItem(q);

        // Some page objects are invisible initially. Make visible.
        item->setVisible(true);
    }
    q->setActive(true);
}

/*!
  \internal
  Hide page object.
 */
void UCPageWrapperPrivate::deactivate()
{
    q_func()->setActive(false);
}

/*!
 Converts the QObject into a QQuickItem, if that is not possible
 the pointer is destroyed and nullptr is returned
 */
QQuickItem *UCPageWrapperPrivate::toItem(QObject *theObject, bool canDelete)
{
    Q_Q(UCPageWrapper);
    QQuickItem *theItem = qobject_cast<QQuickItem *>(theObject);
    if (!theItem) {
        //the returned object has to be a Item but if its a Object
        //do not leak the pointer
        if (canDelete && theObject) delete theObject;

        qmlInfo(q) << "PageWrapper only supports components that are derived from Item";;
        return nullptr;
    }
    return theItem;
}

/*!
 Initialize the Item and make sure the object property is set
 */
void UCPageWrapperPrivate::initItem(QQuickItem *theItem)
{
    Q_Q(UCPageWrapper);
    if (theItem) {
        theItem->setParentItem(q);
        copyProperties(theItem);
        q->setObject(theItem);
    }
}

/*!
  \internal
  Hide page object.
 */
void UCPageWrapperPrivate::copyProperties(QObject *target)
{
    if (!target)
        return;

    Q_Q(UCPageWrapper);
    // copy the properties to the page object
    QVariantMap propMap = m_properties.toMap();
    QVariantMap::const_iterator i = propMap.constBegin();

    QQmlContext *context = m_itemContext ? m_itemContext : qmlContext(target);
    for (;i != propMap.constEnd(); i++) {
        if (!QQmlProperty::write(target, i.key(), i.value(), context)) {
            qmlInfo(q) << "Could not assign value: "<<i.value()<<"to property: "<<i.key();
        }
    }
}

/*!
  \internal
 Creates the Incubator wrapper object
 */
void UCPageWrapperPrivate::createIncubator()
{
    Q_Q(UCPageWrapper);
    if (m_incubator)
        destroyIncubator();

    m_incubator = new UCPageWrapperIncubator(QQmlIncubator::Asynchronous, q);
    Q_EMIT q->incubatorChanged(m_incubator);
}

/*!
  \internal
 Destroys the Incubator wrapper if it was set before
 */
void UCPageWrapperPrivate::destroyIncubator()
{
    if (m_incubator) {
        m_incubator->deleteLater();
        m_incubator = nullptr;
        Q_EMIT q_func()->incubatorChanged(m_incubator);
    }
}

/*!
 Handles all state transitions and advances the statemachine until the
 component is either instantiated or an error occured
 */
void UCPageWrapperPrivate::nextStep()
{
    Q_Q(UCPageWrapper);
    switch(m_state) {
        case Waiting:{
            loadComponentState();
            break;
        }
        case LoadingComponent:{
            createObjectState();
            break;
        }
        case CreatingObject:{
            finalizeObjectIfReady();
            break;
        }
        case NotifyPageLoaded: {
            // emit pageWrapper's pageLoaded signal to complete page activation
            m_state = Ready;
            Q_EMIT q->pageLoaded();
            break;
        }
        default:
            break; // do nothing
    }
}

void UCPageWrapperPrivate::loadComponentState()
{
    Q_Q(UCPageWrapper);
    m_state = LoadingComponent;

    if (m_reference.canConvert<QQmlComponent *>()) {

        //m_reference points to a Component already, make sure we do not
        //delete it later on
        m_ownsComponent = false;
        m_component = m_reference.value<QQmlComponent *>();

    } else if (m_reference.canConvert<QString>()) {

        //m_reference contains a URL to the Component we have to load, in this
        //case we need to destroy the component ourselves lateron
        QQmlComponent::CompilationMode cMode = m_synchronous ? QQmlComponent::PreferSynchronous :
                                                               QQmlComponent::Asynchronous;
        QUrl componentUrl = QUrl(m_reference.toString());
        m_ownsComponent = true;
        m_component = new QQmlComponent(qmlEngine(q), componentUrl, cMode);

    } else if (m_reference.canConvert<QQuickItem *>()) {
        //the object is owned by JS
        setCanDestroy(false);

        //m_reference is already a Item, we just need to copy the properties and we are done
        QQuickItem *theItem = m_reference.value<QQuickItem *>();
        initItem(theItem);

        //proceed to final step
        m_state = NotifyPageLoaded;
        nextStep();
        return;
    } else {
        m_state = Error;
        qmlInfo(q) << "PageWrapper.reference contains unsupported data";
        return;
    }

    if (m_component) {
        if (!m_synchronous) {
            //The Incubator needs to be created ahead of time because QML code assumes its valid right away
            createIncubator();
        }

        //check if the component is already available
        if (m_component->status() != QQmlComponent::Loading)
            nextStep();
        else {
            //async behaviour, advance to the next state once the component was loaded
            QSharedPointer<QMetaObject::Connection> connHandle(new QMetaObject::Connection);
            auto asyncCallback = [this, connHandle](){
                if(m_component->status() != QQmlComponent::Loading) {
                    QObject::disconnect(*connHandle);
                    nextStep();
                }
            };

            *connHandle = QObject::connect(m_component, &QQmlComponent::statusChanged,asyncCallback);
        }
    }
}

void UCPageWrapperPrivate::createObjectState()
{
    Q_Q(UCPageWrapper);
    if (m_component->status() == QQmlComponent::Error) {
        qmlInfo(q) << m_component->errors();
        m_state = Error;
        return; //full stop
    }

    //the Component was created, next step is to create a Item instance
    m_state = CreatingObject;

    //Object has C++ ownership
    setCanDestroy(true);

    // create context
    // use creation context as parent to create the context we load the style item with
    QQmlContext *creationContext = m_component->creationContext();
    if (!creationContext) {
        creationContext = qmlContext(q);
    }
    if (creationContext && !creationContext->isValid()) {
        // we are having the changes in the component being under deletion

        qmlInfo(q) << "Could not get creation context";
        m_state = Error;
        return;
    }

    m_itemContext = new QQmlContext(creationContext);

    if (m_synchronous) {
        QQuickItem *theItem = toItem(m_component->beginCreate(m_itemContext));
        if (theItem) {
            initItem(theItem);
            m_itemContext->setParent(theItem);
            m_itemContext = nullptr;
            m_component->completeCreate();
            m_state = NotifyPageLoaded;
            nextStep();
        } else {
            delete m_itemContext;
            m_itemContext = nullptr;
            m_state = Error;
        }
    } else {
        //connect the change signal first so we definately catch the signal even when the creation finishes right away
        QObject::connect(m_incubator, SIGNAL(enterOnStatusChanged()),
                         q, SLOT(nextStep()));

        //make sure we intialize the item before it is instantiated
        QSharedPointer<QMetaObject::Connection> connHandle(new QMetaObject::Connection);
        auto asyncCallback = [this, connHandle](QObject *target){
            QObject::disconnect(*connHandle);
            QQuickItem *theItem = toItem(target, false);
            if (theItem) {
                initItem(theItem);
                m_itemContext->setParent(theItem);
                m_itemContext = nullptr;
            }
        };
        *connHandle = QObject::connect(m_incubator, &UCPageWrapperIncubator::initialStateRequested, asyncCallback);

        m_component->create(*m_incubator, m_itemContext);
    }
}

void UCPageWrapperPrivate::finalizeObjectIfReady()
{
    Q_Q(UCPageWrapper);
    if(m_incubator->status() == QQmlIncubator::Ready) {

        QObject::disconnect(m_incubator, SIGNAL(enterOnStatusChanged()),
                         q, SLOT(nextStep()));

        QQuickItem *theItem = toItem(m_incubator->object());
        if (theItem) {
            //this code needs to be executed after the Incubator has executed the JS callback
            QSharedPointer<QMetaObject::Connection> sharedConn(new QMetaObject::Connection);
            auto asyncCallback = [this, sharedConn](){
                QObject::disconnect(*sharedConn);
                destroyIncubator();

                m_state = NotifyPageLoaded;
                nextStep();
            };
            *sharedConn = QObject::connect(m_incubator, &UCPageWrapperIncubator::statusHasChanged,asyncCallback);

            return;
        } else {
            m_state = Error;
        }
    } else if(m_incubator->status() == QQmlIncubator::Error) {
        m_state = Error;
        qmlInfo(q) << m_incubator->errors();
    } else if (m_incubator->status() == QQmlIncubator::Null) {
        //page loading was cancled
        reset();
        return;
    }

    // cleanup
    if(m_incubator->status() != QQmlIncubator::Loading) {
        //if we reach this point there was a error, make sure the item context is properly
        //destroyed
        if (m_itemContext) {
            delete m_itemContext;
            m_itemContext = nullptr;
        }
        destroyIncubator();
    }
}

void UCPageWrapperPrivate::onActiveChanged()
{
    q_func()->setVisible(m_active);
}

/*!
    \internal
    \qmltype PageWrapper
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \brief Internal class used by \l PageStack
*/
UCPageWrapper::UCPageWrapper(QQuickItem *parent)
    : UCPageTreeNode((* new UCPageWrapperPrivate), parent)
{
    d_func()->init();
}

UCPageWrapper::UCPageWrapper(UCPageWrapperPrivate &dd, QQuickItem *parent)
    : UCPageTreeNode(dd, parent)
{
    d_func()->init();
}

UCPageWrapper::~UCPageWrapper()
{
    d_func()->deactivate();
}

/*!
  \qmlproperty var PageWrapper::reference
  The reference to the page object. This can be the page
  itself (which is an Item), but also a url pointing to a QML file.
 */
QVariant UCPageWrapper::reference() const
{
    return d_func()->m_reference;
}

void UCPageWrapper::setReference(const QVariant &reference)
{
    Q_D(UCPageWrapper);
    if (d->m_reference == reference)
        return;

    d->m_reference = reference;

    d->deactivate();
    d->initPage();

    if (d->m_active && d->m_reference.isValid()) {
        if ((d->m_incubator && d->m_incubator->status() == QQmlIncubator::Ready) || d->m_object) {
            d->activate();
        } else {
            // asynchronous, connect page activation

            //make sure this connection is destroyed once it was fired. Using a stack
            //variable does not work because it would not yet be initialized when the variable
            //is captured
            QSharedPointer<QMetaObject::Connection> sharedConn(new QMetaObject::Connection);
            auto asyncCallback = [d, sharedConn](){
                d->activate();
                QObject::disconnect(*sharedConn);
            };

            *sharedConn = QObject::connect(this, &UCPageWrapper::pageLoaded,asyncCallback);
        }
    }


    Q_EMIT referenceChanged(reference);
}

/*!
  \qmlproperty Item PageWrapper::object
  The initialized page object, or null if the object needs to be created.
 */
QQuickItem *UCPageWrapper::object() const
{
    return d_func()->m_object;
}

void UCPageWrapper::setObject(QQuickItem *object)
{
    Q_D(UCPageWrapper);
    if (d->m_object == object)
        return;

    d->m_object = object;
    Q_EMIT objectChanged(object);
}

/*!
  \qmlproperty bool PageWrapper::canDestroy
  This property will be true if \l object holds an object that was created
  from the given reference, and thus can be destroyed when no the page is deactivated.
 */
bool UCPageWrapper::canDestroy() const
{
    return d_func()->m_canDestroy;
}

void UCPageWrapperPrivate::setCanDestroy(bool canDestroy)
{
    Q_Q(UCPageWrapper);
    if (m_canDestroy == canDestroy)
        return;

    m_canDestroy = canDestroy;
    Q_EMIT q->canDestroyChanged(canDestroy);
}

/*!
  \qmlproperty int PageWrapper::column
  Column number in AdaptivePageLayout.
*/
int UCPageWrapper::column() const
{
    return d_func()->m_column;
}

void UCPageWrapper::setColumn(int column)
{
    Q_D(UCPageWrapper);
    if (d->m_column == column)
        return;

    d->m_column = column;
    Q_EMIT columnChanged(column);
}

/*!
  \qmlproperty Item PageWrapper::parentPage
  Parent page.
*/
QQuickItem *UCPageWrapper::parentPage() const
{
    return d_func()->m_parentPage;
}

void UCPageWrapper::setParentPage(QQuickItem *parentPage)
{
    Q_D(UCPageWrapper);
    if (d->m_parentPage == parentPage)
        return;

    d->m_parentPage = parentPage;
    Q_EMIT parentPageChanged(parentPage);
}

/*!
  \qmlproperty var PageWrapper::incubator
  Incubator for the asynchronous page creation
  */
QObject *UCPageWrapper::incubator() const
{
    return d_func()->m_incubator;
}

/*!
  \internal
  \qmlmethod PageWrapper::destroyObject()
  Destroy \l object. Only call this function if \l canDestroy
 */
void UCPageWrapper::destroyObject()
{
    Q_D(UCPageWrapper);
    if (d->m_canDestroy && d->m_object) {
        d->m_object->deleteLater();
        d->m_canDestroy = false;
        setObject(nullptr);
    }
}

void UCPageWrapper::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &data)
{
    if (change == ItemParentHasChanged) {
        //make sure we always fill the parents space completely
        QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
        if (data.item) {
            anchors->setFill(data.item);
        } else {
            anchors->resetFill();
        }
    }
    UCStyledItemBase::itemChange(change, data);
}

/*!
  \qmlproperty Item PageWrapper::parentWrapper
  Parent PageWrapper or the parentPage.
  */
QQuickItem *UCPageWrapper::parentWrapper() const
{
    return d_func()->m_parentWrapper;
}

void UCPageWrapper::setParentWrapper(QQuickItem *parentWrapper)
{
    Q_D(UCPageWrapper);
    if (d->m_parentWrapper == parentWrapper)
        return;

    d->m_parentWrapper = parentWrapper;
    Q_EMIT parentWrapperChanged(parentWrapper);
}

/*!
  \qmlproperty Item PageWrapper::pageHolder
  Page holder in AdaptivePageLayout.
  */
QQuickItem *UCPageWrapper::pageHolder() const
{
    return d_func()->m_pageHolder;
}

void UCPageWrapper::setPageHolder(QQuickItem *pageHolder)
{
    Q_D(UCPageWrapper);
    if (d->m_pageHolder == pageHolder)
        return;

    d->m_pageHolder = pageHolder;
    Q_EMIT pageHolderChanged(pageHolder);
}

/*!
  \qmlproperty bool PageWrapper::synchronous
  Instructs to load the page synchronously or not. Used by AdaptivePageLayout.
  True by default to keep PageStack integrity.
  */
bool UCPageWrapper::synchronous() const
{
    return d_func()->m_synchronous;
}

void UCPageWrapper::setSynchronous(bool synchronous)
{
    Q_D(UCPageWrapper);
    if (d->m_synchronous == synchronous)
        return;

    d->m_synchronous = synchronous;
    Q_EMIT synchronousChanged(synchronous);
}

/*!
  \qmlmethod bool PageWrapper::childOf(Item)
  Returns true if the current PageWrapper is a child of the given page
  */
bool UCPageWrapper::childOf(QQuickItem *page)
{
    Q_D(UCPageWrapper);
    if (d->m_parentPage == page) return true;
    if (page && d->m_parentWrapper) {
        UCPageWrapper *wrapper = qobject_cast<UCPageWrapper *>(d->m_parentWrapper);
        while (wrapper) {
            if (wrapper->object() == page) {
                return true;
            }
            wrapper = qobject_cast<UCPageWrapper *>(wrapper->parentWrapper());
        }
    }
    return false;
}

/*!
  \qmlproperty var PageWrapper::properties
  Properties are use to initialize a new object, or if reference
  is already an object, properties are copied to the object when activated.
  Set properties before setting the reference.
 */
QVariant UCPageWrapper::properties() const
{
    return d_func()->m_properties;
}

void UCPageWrapper::setProperties(const QVariant &properties)
{
    Q_D(UCPageWrapper);
    if (d->m_properties == properties)
        return;

    d->m_properties = properties;
    Q_EMIT propertiesChanged(properties);
}


void UCPageWrapper::setVisible2(bool visible)
{
    Q_D(UCPageWrapper);

    //remove the binding to visible
    if (!(d->m_flags & UCPageWrapperPrivate::CustomVisible)) {
        disconnect(this, SIGNAL(activeChanged(bool)),
                   this, SLOT(onActiveChanged()));
    }

    d->m_flags |= UCPageWrapperPrivate::CustomVisible;
    setVisible(visible);
}

UCTheme *UCPageWrapper::getTheme2()
{
    return getTheme();
}

void UCPageWrapper::setTheme2(UCTheme *theme)
{
    setTheme(theme);
}

void UCPageWrapper::resetTheme2()
{
    resetTheme();
}

/*!
  \qmlsignal PageWrapper::pageLoaded()
  Signal emitted when incubator completes page loading.
 */

UT_NAMESPACE_END

#include "moc_ucpagewrapper.cpp"
