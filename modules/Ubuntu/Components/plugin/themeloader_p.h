#ifndef THEMELOADER_P_H
#define THEMELOADER_P_H

#include <QObject>

class QQmlEngine;
class StyleTreeNode;

class ThemeLoader {
public:
    ThemeLoader(QQmlEngine *engine, QObject *parent = 0);
    virtual ~ThemeLoader();
    virtual StyleTreeNode *loadTheme(const QUrl &path) = 0;
protected:
    QQmlEngine *m_engine;
};

Q_DECLARE_INTERFACE(ThemeLoader, "org.qt-project.Qt.ThemeLoader")

#endif // THEMELOADER_P_H
