#ifndef STYLE_P_H
#define STYLE_P_H

#include "style.h"

class StylePrivate
{
    Q_DECLARE_PUBLIC(Style)

public:
    StylePrivate(Style *qq);

    Style *q_ptr;

    QDeclarativeItem *target;
    QDeclarativeComponent *delegate;
    QList<QObject*> data;
    QList<QDeclarativeState*> styleStates;
    // by default it is the style className, can be modified with styleClass property
    QString styleClass;
    QString instanceId;
    QStringList states;

};

#endif // STYLE_P_H
