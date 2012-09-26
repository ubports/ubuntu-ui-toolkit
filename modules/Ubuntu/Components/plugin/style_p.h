#ifndef STYLE_P_H
#define STYLE_P_H

#include "style.h"

class StyleRulePrivate
{
    Q_DECLARE_PUBLIC(StyleRule)

public:
    StyleRulePrivate(StyleRule *qq);

    StyleRule *q_ptr;

    QDeclarativeComponent *style;
    QDeclarativeComponent *delegate;
    QString selector;
};

#endif // STYLE_P_H
