#ifndef TESTEXTRAS_H
#define TESTEXTRAS_H

#include <QtCore/QObject>

class QQuickItem;
class QTouchDevice;
class TestExtras : public QObject
{
    Q_OBJECT
public:
    explicit TestExtras(QObject *parent = 0);

public Q_SLOTS:
    static void registerTouchDevice();
    static void touchPress(int touchId, QQuickItem *item, const QPoint &point);
    static void touchRelease(int touchId, QQuickItem *item, const QPoint &point);
    static void touchClick(int touchId, QQuickItem *item, const QPoint &point);
    static void touchLongPress(int touchId, QQuickItem *item, const QPoint &point);
    static void touchDoubleClick(int touchId, QQuickItem *item, const QPoint &point);
    static void touchMove(int touchId, QQuickItem *item, const QPoint &point);
    static void touchDrag(int touchId, QQuickItem *item, const QPoint &from, const QPoint &delta, int steps = 5);

private:
    static QTouchDevice *m_touchDevice;

    static bool checkTouchDevice(const char *func);
};

#endif // TESTEXTRAS_H
