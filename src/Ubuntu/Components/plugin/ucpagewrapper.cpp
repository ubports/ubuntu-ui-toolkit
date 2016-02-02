#include "ucpagewrapper.h"
#include "ucpagewrapper_p.h"
#include "ucpagewrapperincubator_p.h"

#include <QQmlEngine>
#include <QQmlContext>

UCPageWrapperPrivate::UCPageWrapperPrivate() :
    m_object(nullptr),
    m_parentPage(nullptr),
    m_parentWrapper(nullptr),
    m_pageHolder(nullptr),
    m_incubator(nullptr),
    m_localComponent(nullptr),
    m_column(0),
    m_canDestroy(false),
    m_synchronous(true)
{ }

UCPageWrapperPrivate::~UCPageWrapperPrivate()
{
    reset();
}

void UCPageWrapperPrivate::init()
{
    Q_Q(UCPageWrapper);
    q->setActive(false);

    //this will disable the superclass' binding
    q->setVisible(false);

    //and we introduce our own one here
    QObject::connect(q, SIGNAL(visibleChanged()),
                     q, SLOT(onVisibleChanged()));

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
        if (m_reference.isValid()) {
            if (m_active) {
                this->activate();
            } else {
                this->deactivate();
            }
        }
    });
}

void UCPageWrapperPrivate::initPage()
{
    Q_Q(UCPageWrapper);

    //make sure we are clean
    reset();

    QQmlComponent *pageComponent = nullptr;
    if (m_reference.canConvert<QQmlComponent *>()) {
        pageComponent = m_reference.value<QQmlComponent *>();
    } else if (m_reference.canConvert<QString>()) {
        if (m_synchronous) {
            m_localComponent = pageComponent = new QQmlComponent(qmlEngine(q), QUrl::fromLocalFile(m_reference.toString()),QQmlComponent::PreferSynchronous);
        } else {
            m_localComponent = pageComponent = new QQmlComponent(qmlEngine(q), QUrl::fromLocalFile(m_reference.toString()),QQmlComponent::Asynchronous);
        }
    } else if (m_reference.canConvert<QObject *>()) {
        QObject *theObject = m_reference.value<QObject *>();
        theObject->setParent(q);

        q->setObject(theObject);
        q->setCanDestroy(false);
        copyProperties(theObject);
    } else {
        qmlInfo(q) << "PageWrapper.reference contains unsupported data";
        return;
    }

    if (pageComponent) {
        if (pageComponent->status() != QQmlComponent::Loading)
            onComponentStatusChanged(pageComponent);
        else {
            //async behaviour
            QSharedPointer<QMetaObject::Connection> connHandle(new QMetaObject::Connection);
            *connHandle = QObject::connect(pageComponent, &QQmlComponent::statusChanged,
                                           [this, connHandle, pageComponent](){
                if(pageComponent->status() != QQmlComponent::Loading)
                    QObject::disconnect(*connHandle);

                onComponentStatusChanged(pageComponent);
            });
        }
    }
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

    if (m_localComponent) {
        delete m_localComponent;
        m_localComponent = nullptr;
    }
}

void UCPageWrapperPrivate::activate()
{
    Q_Q(UCPageWrapper);
    if (!m_object) {
        initPage();
    }

    //make sure object is valid now
    if (!m_object)
        return;

    // Having the same page pushed multiple times on a stack changes
    // the parent of the page object. Change it back here.
    m_object->setParent(q);

    // Some page objects are invisible initially. Make visible.
    QQuickItem *item = qobject_cast<QQuickItem *>(m_object);
    if (item){
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

void UCPageWrapperPrivate::onComponentStatusChanged(QQmlComponent *pageComponent)
{
    Q_Q(UCPageWrapper);
    if (pageComponent->status() == QQmlComponent::Error) {
        if (m_localComponent) {
            delete m_localComponent;
            m_localComponent = nullptr;
        }
        return; //full stop
    }
    if (m_synchronous) {
        q->setObject(pageComponent->create(qmlContext(q)));
        if (m_localComponent) {
            delete m_localComponent;
            m_localComponent = nullptr;
        }
    } else {
        UCPageWrapperIncubator *incubator = new UCPageWrapperIncubator(QQmlIncubator::Asynchronous, q);
        //connect the change signal first so we definately catch the signal even when the creation finishes right away
        QObject::connect(incubator, SIGNAL(statusHasChanged(int)),
                         q, SLOT(onIncubatorStatusChanged()));

        setIncubator(incubator);
        pageComponent->create(*incubator, qmlContext(q));
    }
}

void UCPageWrapperPrivate::onIncubatorStatusChanged()
{
    Q_Q(UCPageWrapper);

    // emit pageWrapper's pageLoaded signal to complete page activation
    if(m_incubator->status() == QQmlIncubator::Ready) {
        QObject *theObject = m_incubator->object();
        copyProperties(theObject);

        q->setObject(theObject);

        if (theObject)
            Q_EMIT q->pageLoaded();

    } else if(m_incubator->status() == QQmlIncubator::Error) {
        qmlInfo(q) << m_incubator->errors();
    }

    // cleanup of ready or error
    if(m_incubator->status() != QQmlIncubator::Loading) {
        setIncubator(nullptr);
        m_incubator->deleteLater();
    }
}

void UCPageWrapperPrivate::onVisibleChanged()
{
    _q_activeBinding(q_func()->isVisible());
}

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

void UCPageWrapper::setActive(bool active)
{
    Q_D(UCPageWrapper);

    //remove the binding to visible
    if (!(d->m_flags & UCPageWrapperPrivate::CustomPageWrapperActive)) {
        disconnect(this, SIGNAL(visibleChanged()),
                   this, SLOT(onVisibleChanged()));
    }

    d->m_flags |= UCPageWrapperPrivate::CustomPageWrapperActive;
    UCPageTreeNode::setActive(active);
}

QObject *UCPageWrapper::incubator() const
{
    return d_func()->m_incubator;
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

