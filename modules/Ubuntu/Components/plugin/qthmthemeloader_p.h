#ifndef QTHMTHEMELOADER_P_H
#define QTHMTHEMELOADER_P_H

#include "themeengine_p.h"
#include "themeloader_p.h"

// CSS-like theme loader
class QthmThemeLoader;
typedef bool (*QthmThemeParserCallback)(QthmThemeLoader *loader, QTextStream &stream);

class QthmThemeLoader : public ThemeLoader {
    Q_INTERFACES(ThemeLoader)
public:
    QthmThemeLoader(QQmlEngine *engine);
    virtual ~QthmThemeLoader(){}
    StyleTreeNode *loadTheme(const QUrl &path);

private:
    StyleTreeNode *styleTree;

private:

    static QString readChar(QTextStream &stream, const QRegExp &bypassTokens = QRegExp("[ \t\r\n]"));
    static QString readTillToken(QTextStream &stream, const QRegExp &tokens, const QRegExp &bypassTokens = QRegExp(), bool excludeToken = true);
    bool handleSelector(const Selector &path, const QString &declarator);
    void normalizeStyles();
    bool parseTheme(const QUrl &url);
    bool buildStyleTree();

    // @-rule handlers
    static bool handleImport(QthmThemeLoader *loader, QTextStream &stream);
    static bool handleQmlMapping(QthmThemeLoader *loader, QTextStream &stream);
    static bool handleQmlImport(QthmThemeLoader *loader, QTextStream &stream);

    QString imports;
    QHash<QString, QthmThemeParserCallback> rules;
    QHash<QString, QPair<QString, QString> > qmlMap;
    QHash<Selector, QHash<QString, QString> > selectorTable;
};



#endif // QTHMTHEMELOADER_P_H
