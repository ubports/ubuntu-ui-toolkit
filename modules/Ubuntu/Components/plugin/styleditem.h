#ifndef STYLEDITEM_H
#define STYLEDITEM_H

#include <QDeclarativeItem>
#include <QDeclarativeListProperty>

class Style;

class StyledItemPrivate;
class StyledItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(StyledItem)

    // for some reason the derived's state is not exposed...???
    //Q_PROPERTY(QString state READ state )
    Q_PROPERTY(QString uid READ uid WRITE setUid NOTIFY uidChanged)
    Q_PROPERTY(Style *style READ activeStyle NOTIFY styleChanged FINAL)
    Q_PROPERTY(QDeclarativeListProperty<Style> customStyles READ customStyles)

public:
    StyledItem(QDeclarativeItem *parent = 0);
    ~StyledItem();

    void componentComplete();
    
signals:

    void styleClassChanged();
    void styleChanged();
    
public slots:

private: // getter/setter
    QString uid() const;
    void setUid(const QString &uid);
    Style *activeStyle() const;
    QDeclarativeListProperty<Style> customStyles();

private: //members
    Q_DECLARE_PRIVATE(StyledItem)
    QScopedPointer<StyledItemPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_updateCurrentStyle(const QString &))
};

QML_DECLARE_TYPE(StyledItem)

#endif // STYLEDITEM_H
