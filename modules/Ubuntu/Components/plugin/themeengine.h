#ifndef THEMEENGINE_H
#define THEMEENGINE_H

#include <QObject>
#include <QHash>

class Style;
class QDeclarativeEngine;
class QDeclarativeItem;

typedef QHash<QString, Style*> StyleStateHash;
typedef QHash<QString, StyleStateHash> StyleHash;

class ThemeEngine : public QObject
{
    Q_OBJECT
public:
    explicit ThemeEngine(QObject *parent = 0);
    ~ThemeEngine();

    static ThemeEngine* instance();
    static void initialize(QDeclarativeEngine *engine);
    static Style *lookupStateStyle(const QString &styleClass, const QString &state);

    Q_INVOKABLE QDeclarativeItem *root();
    Q_INVOKABLE QDeclarativeItem *item(const QString &id);

    //utility function
signals:
    
public slots:

private:
    void loadTheme(const QString &themeFile);
    void buildStyleCache(QObject *style);
    void updateStyle(StyleHash::const_iterator &item, const QString &styleClass, Style *style);
    void addStyle(const QString &styleClass, Style *style);

private: //members
    QDeclarativeEngine *m_engine;
    StyleHash m_styleCache;
};

#endif // THEMEENGINE_H
