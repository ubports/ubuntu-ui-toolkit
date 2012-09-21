#ifndef STYLE_P_H
#define STYLE_P_H

#include "style.h"

class StylePrivate
{
    Q_DECLARE_PUBLIC(Style)

public:
    StylePrivate(Style *qq);

    Style *q_ptr;

    QDeclarativeComponent *style;
    QDeclarativeComponent *visuals;
    QList<Style*> data;
    QString selector;
};

#endif // STYLE_P_H
