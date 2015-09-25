#ifndef UCLISTITEMLAYOUT_H
#define UCLISTITEMLAYOUT_H

#include "ucslotslayout.h"

class UCListItemLayout : public UCSlotsLayout
{
    Q_OBJECT
    Q_PROPERTY(QQuickText *title READ title CONSTANT FINAL)
    Q_PROPERTY(QQuickText *subtitle READ subtitle CONSTANT FINAL)
    Q_PROPERTY(QQuickText *summary READ summary CONSTANT FINAL)

public:
    explicit UCListItemLayout(QQuickItem *parent = 0);

    QQuickText *title();
    QQuickText *subtitle();
    QQuickText *summary();
};

#endif // UCLISTITEMLAYOUT_H
