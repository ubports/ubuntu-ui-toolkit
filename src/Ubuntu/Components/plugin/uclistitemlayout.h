#ifndef UCLISTITEMLAYOUT_H
#define UCLISTITEMLAYOUT_H

#include "ucslotslayout.h"

class UCListItemLayout : public UCSlotsLayout
{
    Q_OBJECT
    Q_PROPERTY(UCLabel *title READ title CONSTANT FINAL)
    Q_PROPERTY(UCLabel *subtitle READ subtitle CONSTANT FINAL)
    Q_PROPERTY(UCLabel *summary READ summary CONSTANT FINAL)

public:
    explicit UCListItemLayout(QQuickItem *parent = 0);

    UCLabel *title();
    UCLabel *subtitle();
    UCLabel *summary();
};

#endif // UCLISTITEMLAYOUT_H
