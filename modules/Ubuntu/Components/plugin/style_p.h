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
    QDeclarativeComponent *delegate;
    QList<QObject*> data;
    // by default it is the style className, can be modified with styleClass property
    QString styleClass;
    QString instanceId;
    QString selector;

};

#endif // STYLE_P_H
