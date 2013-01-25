#ifndef QQUICKCLIPBOARD_P_H
#define QQUICKCLIPBOARD_P_H

#include "qquickclipboard.h"
#include <QtGui/QClipboard>

class QQuickMimeData;
class QQuickClipboardPrivate {
    Q_DECLARE_PUBLIC(QQuickClipboard)
public:
    QQuickClipboardPrivate(QQuickClipboard *qq);

    QQuickClipboard *q_ptr;
    QClipboard *clipboard;
    QClipboard::Mode mode;
    QQuickMimeData *data;
};

#endif // QQUICKCLIPBOARD_P_H
