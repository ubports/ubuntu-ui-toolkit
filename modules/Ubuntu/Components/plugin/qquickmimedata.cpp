
#include <QtCore/QMimeData>
#include <QtCore/QMimeDatabase>
#include <QtCore/QMimeType>
#include "qquickmimedata.h"

#include <QDebug>

QQuickMimeData::QQuickMimeData(QObject *parent) :
    QObject(parent),
    m_refData(false),
    m_localData(false),
    m_mimeData(new QMimeData)
{
}
QQuickMimeData::QQuickMimeData(const QMimeData *refData, bool localData, QObject *parent) :
    QObject(parent),
    m_refData(true),
    m_localData(localData),
    m_mimeData(const_cast<QMimeData*>(refData))
{
}
QQuickMimeData::~QQuickMimeData()
{
    if (m_mimeData)
        qDebug() << m_mimeData->parent();
    if (!m_refData && m_mimeData && !m_mimeData->parent())
    //if (!m_refData && m_mimeData)
        delete m_mimeData;
    m_mimeData = 0;
}


void QQuickMimeData::fromMimeData(const QMimeData *data)
{
    // copy
    if (!data)
        return;
    if (!m_refData)
        delete m_mimeData;
    m_mimeData = const_cast<QMimeData*>(data);
}

QMimeData *QQuickMimeData::toMimeData()
{
    QMimeData *ret = m_mimeData;
    if (!m_refData) {
        m_mimeData = new QMimeData;
        // copy data so we keep the properties as they were
        Q_FOREACH(const QString &format, ret->formats()) {
            m_mimeData->setData(format, ret->data(format));
        }
        // set teh color separately
        m_mimeData->setColorData(ret->colorData());
    }
    return ret;
}

QStringList QQuickMimeData::formats() const
{
    return (m_mimeData) ? m_mimeData->formats() : QStringList();
}
QString QQuickMimeData::text() const
{
    return m_mimeData ? m_mimeData->text() : QString();
}
void QQuickMimeData::setText(const QString &text)
{
    if (!m_refData) {
        m_mimeData->setText(text);
        Q_EMIT textChanged();
    }
}
QString QQuickMimeData::html() const
{
    return m_mimeData ? m_mimeData->html() : QString();
}
void QQuickMimeData::setHtml(const QString &html)
{
    if (!m_refData) {
        m_mimeData->setHtml(html);
        Q_EMIT htmlChanged();
    }
}
QList<QUrl> QQuickMimeData::urls() const
{
    return m_mimeData ? m_mimeData->urls() : QList<QUrl>();
}
void QQuickMimeData::setUrls(const QList<QUrl> &urls)
{
    if (!m_refData) {
        m_mimeData->setUrls(urls);
        Q_EMIT urlsChanged();
    }
}
QColor QQuickMimeData::color() const
{
    return (m_mimeData) ? qvariant_cast<QColor>(m_mimeData->colorData()) : QColor();
}
void QQuickMimeData::setColor(const QColor &color)
{
    if (!m_refData) {
        m_mimeData->setColorData(color);
        Q_EMIT colorChanged();
    }
}

QVariant QQuickMimeData::mimeData() const
{
    if (!m_mimeData)
        return QVariant();
    QVariantList ret;
    Q_FOREACH(const QString &format, formats()) {
        ret << format;
        ret << QVariant(m_mimeData->data(format));
    }
    return QVariant::fromValue(ret);
}

static bool setMimeType(QMimeData *mimeData, QVariantList &mlist)
{
    bool ret = false;
    QMimeDatabase db;
    for (int i = 0; i < mlist.length() / 2; i++) {
        QString type = mlist[2 * i].toString();
        if (db.mimeTypeForName(type).isValid() || type == "application/x-color") {
            QByteArray data = mlist[2 * i + 1].toByteArray();
            mimeData->setData(type, data);
            ret = true;
        } else if (type == "application/x-color") {
            // for some reason colors are not taken in other way...
            mimeData->setColorData(mlist[2 * i + 1]);
            ret = true;
        }
    }
    return ret;
}

void QQuickMimeData::setMimeData(const QVariant &mimeData)
{
    if (m_refData)
        return;

    QMimeDatabase db;
    bool emitSignal = false;

    if (mimeData.type() == QVariant::List) {
        // the list can be a list of URLs or a list of (type, data) pairs or a
        // plain list formatted so that every 2k element describes a type and
        // every 2k+1 defines the data
        QVariantList mlist= mimeData.toList();
        if (mlist[0].type() == QVariant::List) {
            Q_FOREACH(QVariant item, mlist) {
                // should not have more than two elements
                QVariantList block = item.toList();
                if (setMimeType(m_mimeData, block))
                    emitSignal = true;
            }
        } else {
            QUrl url(mlist[0].toUrl());
            if (!url.scheme().isEmpty()) {
                // we have a list or URLs, push them
                QList<QUrl> ulist;
                Q_FOREACH(QVariant v, mlist) {
                    ulist << v.toUrl();
                }
                setUrls(ulist);
                emitSignal = true;
            } else if (!(mlist.length() % 2)) {
                // the count of the list is a pair number, so we might have the
                // mime (type,data) pairs
                emitSignal = setMimeType(m_mimeData, mlist);
            }
        }
    } else if (mimeData.type() == QVariant::Color){
        // color must e handled separately
        m_mimeData->setColorData(mimeData);
        emitSignal = true;
    } else {
        // try to detect the mime data within the variant
        // text/plain, text/html and application/x-color types
        QByteArray data = mimeData.toByteArray();
        QMimeType type = db.mimeTypeForData(mimeData.toByteArray());

        if (type.isValid()) {
            m_mimeData->setData(type.name(), data);
            emitSignal = true;
        } else {
            qWarning() << "UNHANDLED" << mimeData;
        }
    }

    if (emitSignal)
        Q_EMIT dataChanged();
}


