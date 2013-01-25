#ifndef QQUICKCLIPBOARD_H
#define QQUICKCLIPBOARD_H

#include <QtCore/QObject>
#include <QtQml/QJSEngine>
#include <QtQml/QQmlEngine>

class QQuickMimeData;
class QQmlEngine;
class QJSEngine;
class QQuickClipboardPrivate;
class QQuickClipboard : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickMimeData *data READ data NOTIFY dataChanged)
public:
    explicit QQuickClipboard(QObject *parent = 0);

    Q_INVOKABLE QQuickMimeData *newData();

private: //getter/setter
    QQuickMimeData *data() const;
    
Q_SIGNALS:
    void dataChanged();
    
public Q_SLOTS:
    void push(const QVariant& data);
    void clear();

private:
    Q_DECLARE_PRIVATE(QQuickClipboard)
    QScopedPointer<QQuickClipboardPrivate> d_ptr;
};

/*
 * Registration function for the Clipboard type
 */
static QObject *registerClipboard(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    QQuickClipboard *clipboard = new QQuickClipboard;
    return clipboard;
}


#endif // QQUICKCLIPBOARD_H
