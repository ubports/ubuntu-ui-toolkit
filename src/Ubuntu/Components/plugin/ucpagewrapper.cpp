#include "ucpagewrapper.h"
#include "ucpagewrapper_p.h"
#include "ucpagewrapperincubator_p.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QtQuick/private/qquickitem_p.h>

UCPageWrapperPrivate::UCPageWrapperPrivate() :
    m_object(nullptr),
    m_parentPage(nullptr),
    m_parentWrapper(nullptr),
    m_pageHolder(nullptr),
    m_incubator(nullptr),
    m_component(nullptr),
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

    q->setActive(false);
    q->setVisible(false);

    //bind the value of visible to active
    QObject::connect(q, SIGNAL(activeChanged(bool)),
                     q, SLOT(onActiveChanged()));

    QObject::connect(q, &UCPageWrapper::referenceChanged,
                     [this](){
        Q_Q(UCPageWrapper);

        deactivate();
        initPage();

        if (m_active && m_reference.isValid()) {
            if ((m_incubator && m_incubator->status() == QQmlIncubator::Ready) || m_object) {
                activate();
            } else {
                // asynchronous, connect page activation

                //make sure this connection is destroyed once it was fired. Using a stack
                //variable does not work because it would not yet be initialized when the variable
                //is captured
                QSharedPointer<QMetaObject::Connection> sharedConn(new QMetaObject::Connection);
                *sharedConn = QObject::connect(q, &UCPageWrapper::pageLoaded,
                                               [this, sharedConn](){
                    activate();
                    QObject::disconnect(*sharedConn);
                });
            }
        }

    });


    QObject::connect(q, &UCPageWrapper::activeChanged,
                     [this](){
        qDebug()<<"Activating";
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

void UCPageWrapperPrivate::initPage()
{
    //make sure we are clean
    reset();
    nextStep();
}

void UCPageWrapperPrivate::reset()
{
    Q_Q(UCPageWrapper);

    if (m_incubator) {
        //if incubator is READY the object() is not deleted
        if (m_incubator->status() == QQmlIncubator::Ready && m_incubator->object()) {
            m_incubator->object()->deleteLater();
        }
        m_incubator->clear();
        m_incubator->deleteLater();
        setIncubator(nullptr);
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

    m_state = Waiting;
}

void UCPageWrapperPrivate::activate()
{
    Q_Q(UCPageWrapper);
    if (!m_object && m_state != LoadingComponent && m_state != CreatingObject) {
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

void UCPageWrapperPrivate::deactivate()
{
    q_func()->setActive(false);
}

void UCPageWrapperPrivate::copyProperties(QObject *target)
{
    if (!target)
        return;

    Q_Q(UCPageWrapper);
    // copy the properties to the page object
    QVariantMap propMap = m_properties.toMap();
    QVariantMap::const_iterator i = propMap.constBegin();
    while ( i != propMap.constEnd()) {
        if (!target->setProperty(qPrintable(i.key()), i.value()))
            qmlInfo(q) << "Setting unknown property "<<i.key();
        i++;
    }
}

void UCPageWrapperPrivate::setIncubator(UCPageWrapperIncubator *incubator)
{
    Q_Q(UCPageWrapper);
    if (m_incubator == incubator)
        return;

    m_incubator = incubator;
    Q_EMIT q->incubatorChanged(incubator);
}

void UCPageWrapperPrivate::nextStep()
{
    Q_Q(UCPageWrapper);
    switch(m_state) {
        case Waiting:{
            m_state = LoadingComponent;

            if (m_reference.canConvert<QQmlComponent *>()) {
                m_ownsComponent = false;
                m_component = m_reference.value<QQmlComponent *>();
            } else if (m_reference.canConvert<QString>()) {

                QQmlComponent::CompilationMode cMode = m_synchronous ? QQmlComponent::PreferSynchronous :
                                                                       QQmlComponent::Asynchronous;
                QUrl componentUrl = QUrl(m_reference.toString());
                m_ownsComponent = true;
                m_component = new QQmlComponent(qmlEngine(q), componentUrl, cMode);

                if (!m_synchronous) {
                    //The Incubator needs to be created ahead of time because QML code assumes its valid right away
                    setIncubator(new UCPageWrapperIncubator(QQmlIncubator::Asynchronous, q));
                }
            } else if (m_reference.canConvert<QObject *>()) {
                QObject *theObject = m_reference.value<QObject *>();

                QQuickItem *theItem = qobject_cast<QQuickItem *>(theObject);
                if (theItem)
                    theItem->setParentItem(q);

                copyProperties(theObject);

                q->setCanDestroy(false);
                q->setObject(theObject);

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
                if (m_component->status() != QQmlComponent::Loading)
                    nextStep();
                else {
                    //async behaviour
                    QSharedPointer<QMetaObject::Connection> connHandle(new QMetaObject::Connection);
                    *connHandle = QObject::connect(m_component, &QQmlComponent::statusChanged,
                                                   [this, connHandle](){
                        if(m_component->status() != QQmlComponent::Loading) {
                            QObject::disconnect(*connHandle);
                            nextStep();
                        }
                    });
                }
            }
            break;
        }
        case LoadingComponent:{
            m_state = CreatingObject;
            if (m_component->status() == QQmlComponent::Error) {
                qmlInfo(q) << m_component->errors();
                m_state = Error;
                return; //full stop
            }

            //Object has C++ ownership
            q->setCanDestroy(true);

            if (m_synchronous) {
                QObject *theObject = m_component->create(qmlContext(q));
                if (theObject) {

                    QQuickItem *theItem = qobject_cast<QQuickItem *>(theObject);
                    if (theItem)
                        theItem->setParentItem(q);

                    copyProperties(theObject);
                    q->setObject(theObject);

                    m_state = NotifyPageLoaded;
                    nextStep();
                    return;
                }
                qmlInfo(q) << "Error creating the object";
                m_state = Error;
                return;
            } else {
                //connect the change signal first so we definately catch the signal even when the creation finishes right away
                QObject::connect(m_incubator, SIGNAL(statusHasChanged(int)),
                                 q, SLOT(nextStep()));

                m_component->create(*m_incubator, qmlContext(q));
            }
            break;
        }
        case CreatingObject:{
            if(m_incubator->status() == QQmlIncubator::Ready) {
                QObject *theObject = m_incubator->object();

                QQuickItem *theItem = qobject_cast<QQuickItem *>(theObject);
                if (theItem)
                    theItem->setParentItem(q);

                copyProperties(theObject);
                q->setObject(theObject);

                m_state = NotifyPageLoaded;
                nextStep();

            } else if(m_incubator->status() == QQmlIncubator::Error) {
                m_state = Error;
                qmlInfo(q) << m_incubator->errors();
            }

            //@BUG this will throw unexpected null receiver in gallery
            //is there some code that stores the incubator over time?

            // cleanup if ready or error
            if(m_incubator->status() != QQmlIncubator::Loading) {
                setIncubator(nullptr);
                m_incubator->deleteLater();
            }
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

void UCPageWrapperPrivate::onActiveChanged()
{
    qDebug()<<"Setting visibility to: "<<m_active;
    q_func()->setVisible(m_active);
}

UCPageWrapper::UCPageWrapper(QQuickItem *parent)
    : UCPageTreeNode((* new UCPageWrapperPrivate), parent)
{
    qDebug()<<"Creating PageWrapper";
    d_func()->init();
}

UCPageWrapper::~UCPageWrapper()
{
    d_func()->deactivate();
}

UCPageWrapper::UCPageWrapper(UCPageWrapperPrivate &dd, QQuickItem *parent)
    : UCPageTreeNode(dd, parent)
{
    d_func()->init();
}

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
    Q_EMIT referenceChanged(reference);
}

QObject *UCPageWrapper::object() const
{
    return d_func()->m_object;
}

void UCPageWrapper::setObject(QObject *object)
{
    Q_D(UCPageWrapper);
    if (d->m_object == object)
        return;

    d->m_object = object;
    Q_EMIT objectChanged(object);
}

bool UCPageWrapper::canDestroy() const
{
    return d_func()->m_canDestroy;
}

void UCPageWrapper::setCanDestroy(bool canDestroy)
{
    Q_D(UCPageWrapper);
    if (d->m_canDestroy == canDestroy)
        return;

    d->m_canDestroy = canDestroy;
    Q_EMIT canDestroyChanged(canDestroy);
}

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

QObject *UCPageWrapper::incubator() const
{
    return d_func()->m_incubator;
}

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
        QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
        if (data.item) {
            anchors->setFill(data.item);
        } else {
            anchors->resetFill();
        }
    }
    UCStyledItemBase::itemChange(change, data);
}

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

#include "moc_ucpagewrapper.cpp"

