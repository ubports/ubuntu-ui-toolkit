#ifndef QMLTHEMELOADER_P_H
#define QMLTHEMELOADER_P_H

#include "themeengine_p.h"
#include "themeloader_p.h"

// QML theme loader
class QmlThemeLoader : public QObject, public ThemeLoader {
    Q_OBJECT
    Q_INTERFACES(ThemeLoader)
public:
    QmlThemeLoader(QQmlEngine *engine, QObject *parent = 0);
    virtual ~QmlThemeLoader(){}
    StyleTreeNode *loadTheme(const QUrl &path);

private Q_SLOTS:
    void finalizeThemeLoading();
private:
    QQmlComponent *themeComponent;
    StyleTreeNode *styleTree;
    bool async;
};



#endif // QMLTHEMELOADER_P_H
