#ifndef STYLEDITEM_P_H
#define STYLEDITEM_P_H

#include "styleditem.h"

class StyledItemPrivate {

    Q_DECLARE_PUBLIC(StyledItem)

public:
    StyledItemPrivate(StyledItem *qq);
    //~StyledItemPrivate(){}

    StyledItem *q_ptr;
    bool useCustomStyle;
    Style *activeStyle;
    QString styleClass;
    QString uid;
    QList<Style*> customStyles;

    void setUid(const QString &newUid);

    void _q_updateCurrentStyle(const QString &state);

};


#endif // STYLEDITEM_P_H
