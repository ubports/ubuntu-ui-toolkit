#include "themeengine.h"
#include "style.h"
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtDeclarative/QDeclarativeItem>

#include <QDebug>

Q_GLOBAL_STATIC(ThemeEngine, themeEngine)


ThemeEngine::ThemeEngine(QObject *parent) :
    QObject(parent)
{
}

ThemeEngine::~ThemeEngine()
{
    // needed by the Q_GLOBAL_STATIC
}

ThemeEngine* ThemeEngine::instance()
{
    return themeEngine();
}

void ThemeEngine::initialize(QDeclarativeEngine *engine)
{
    ThemeEngine *theme = themeEngine();
    theme->m_engine = engine;
    // load default theme, and build hash tree
    //theme->loadTheme("/usr/lib/qt4/imports/Ubuntu/Components/DefaultTheme.qml");
    theme->loadTheme("modules/Ubuntu/Components/DefaultTheme.qml");
    // load active theme
    theme->loadTheme("demos/ActiveTheme.qml");
    // TODO: watch theme folders to detect system theme changes
}
Style *ThemeEngine::lookupStateStyle(const QString &styleClass, const QString &state)
{
    ThemeEngine *theme = themeEngine();
    Style *ret = 0;

    StyleHash::const_iterator sh = theme->m_styleCache.find(styleClass);
    if ((sh != theme->m_styleCache.end()) && (sh.key() == styleClass)) {
        // style found, look for the state
        StyleStateHash states(sh.value());
        StyleStateHash::const_iterator i = states.find(state);
        if ((i != states.end()) && (i.key() == state))
            ret = i.value();
    }
    return ret;
}

QDeclarativeItem *ThemeEngine::root()
{

}

QDeclarativeItem *ThemeEngine::item(const QString &id)
{
    QDeclarativeItem *ret = 0;

    return ret;
}


void ThemeEngine::loadTheme(const QString &themeFile)
{
    QDeclarativeComponent *themeComponent = new QDeclarativeComponent(m_engine, QUrl::fromLocalFile(themeFile));
    // TODO: do this async
    while (themeComponent->isLoading());

    if (!themeComponent->isError()) {
        QObject *themeObject = themeComponent->create(m_engine->rootContext());
        if (themeObject) {
            // parse its children for Styles
            buildStyleCache(themeObject);

            // DEBUG: print theme hash
            QHashIterator<QString, StyleStateHash> sh(m_styleCache);
            while (sh.hasNext()) {
                sh.next();
                qDebug() << "Style" << sh.key();
                QHashIterator<QString, Style*> st(sh.value());
                while (st.hasNext()) {
                    st.next();
                    qDebug() << "   state:" << st.key() << "::" << st.value();
                }
            }
        }
    } else {
        qDebug() << "Create theme ERROR:" << themeComponent->errorString();
    }
}

// parses theme for style objects
void ThemeEngine::buildStyleCache(QObject *style)
{
    QList<Style*> styles = style->findChildren<Style*>();
    QString styleClass;
    foreach (Style *pl, styles) {
        // check if we have the style registered already
        styleClass = pl->styleClass();
        StyleHash::const_iterator i = m_styleCache.find(styleClass);
        if ((i != m_styleCache.end()) && (i.key() == styleClass))
            updateStyle(i, styleClass, pl);
        else
            addStyle(styleClass, pl);
    }
}

void ThemeEngine::updateStyle(StyleHash::const_iterator &item, const QString &styleClass, Style *style)
{
    StyleStateHash hash(item.value());
    foreach (const QString &state, style->states()) {
        // check if we have the state already
        StyleStateHash::const_iterator i = hash.find(state);
        if ((i != hash.end()) && (i.key() == state))
            hash[state] = style;
        else
            hash.insert(state, style);
    }
    m_styleCache.insert(styleClass, hash);
}

void ThemeEngine::addStyle(const QString &styleClass, Style *style)
{
    StyleStateHash hash;
    // create entry for each state with the style to ease lookup
    foreach (const QString &state, style->states())
        hash.insert(state, style);
    // insert has in style hash
    m_styleCache.insert(styleClass, hash);
}



