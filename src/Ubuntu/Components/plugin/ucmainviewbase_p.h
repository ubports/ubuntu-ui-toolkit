#ifndef UCMAINVIEWBASE_P_H
#define UCMAINVIEWBASE_P_H

#include "ucpagetreenode_p.h"
#include <QQmlProperty>

class UCMainViewBase;
class UCActionManager;
class UCPopupContext;

class UCMainViewBasePrivate : public UCPageTreeNodePrivate
{
    Q_DECLARE_PUBLIC(UCMainViewBase)

public:
    UCMainViewBasePrivate();
    void init();

    enum PropertyFlags {
        CustomHeaderColor     = 0x01,
        CustomBackgroundColor = 0x02,
        CustomFooterColor     = 0x04
    };

    void _q_headerColorBinding (const QColor &col);
    void _q_footerColorBinding (const QColor &col);

    QString m_applicationName;
    QColor m_headerColor;
    QColor m_backgroundColor;
    QColor m_footerColor;
    UCActionManager *m_actionManager = nullptr;
    UCPopupContext* m_actionContext = nullptr;
    qint8 m_flags;
    bool m_anchorToKeyboard:1;

};

#endif // UCMAINVIEWBASE_P_H

