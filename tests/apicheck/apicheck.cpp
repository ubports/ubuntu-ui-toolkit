/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtQml/qqmlengine.h>
#include <QtQml/private/qqmlmetatype_p.h>
#include <QtQml/private/qqmlopenmetaobject_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquickpincharea_p.h>

#include <QtGui/QGuiApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>
#include <QtCore/private/qobject_p.h>
#include <QtCore/private/qmetaobject_p.h>
#include <QtQml/private/qqmldirparser_p.h>
#include <QJSEngine>

#include <iostream>
#include <algorithm>

#ifdef Q_OS_UNIX
#include <signal.h>
#endif

QString pluginImportPath;
bool verbose = false;
bool creatable = true;

QString currentProperty;
QString inObjectInstantiation;

void collectReachableMetaObjects(const QMetaObject *meta, QSet<const QMetaObject *> *metas, bool extended = false)
{
    if (! meta || metas->contains(meta))
        return;

    // dynamic meta objects can break things badly
    // but extended types are usually fine
    const QMetaObjectPrivate *mop = reinterpret_cast<const QMetaObjectPrivate *>(meta->d.data);
    if (extended || !(mop->flags & DynamicMetaObject))
        metas->insert(meta);

    collectReachableMetaObjects(meta->superClass(), metas);
}

void collectReachableMetaObjects(QObject *object, QSet<const QMetaObject *> *metas)
{
    if (! object)
        return;

    const QMetaObject *meta = object->metaObject();
    if (verbose)
        std::cerr << "Processing object" << qPrintable( meta->className() ) << std::endl;
    collectReachableMetaObjects(meta, metas);

    for (int index = 0; index < meta->propertyCount(); ++index) {
        QMetaProperty prop = meta->property(index);
        if (QQmlMetaType::isQObject(prop.userType())) {
            if (verbose)
                std::cerr << "  Processing property" << qPrintable( prop.name() ) << std::endl;
            currentProperty = QString("%1::%2").arg(meta->className(), prop.name());

            // if the property was not initialized during construction,
            // accessing a member of oo is going to cause a segmentation fault
            QObject *oo = QQmlMetaType::toQObject(prop.read(object));
            if (oo && !metas->contains(oo->metaObject()))
                collectReachableMetaObjects(oo, metas);
            currentProperty.clear();
        }
    }
}

void collectReachableMetaObjects(const QQmlType *ty, QSet<const QMetaObject *> *metas)
{
    collectReachableMetaObjects(ty->metaObject(), metas, ty->isExtendedType());
    if (ty->attachedPropertiesType())
        collectReachableMetaObjects(ty->attachedPropertiesType(), metas);
}

/* We want to add the MetaObject for 'Qt' to the list, this is a
   simple way to access it.
*/
class FriendlyQObject: public QObject
{
public:
    static const QMetaObject *qtMeta() { return &staticQtMetaObject; }
};

/* When we dump a QMetaObject, we want to list all the types it is exported as.
   To do this, we need to find the QQmlTypes associated with this
   QMetaObject.
*/
static QHash<QByteArray, QSet<const QQmlType *> > qmlTypesByCppName;

// No different versioning possible for a composite type.
static QMap<QString, const QQmlType * > qmlTypesByCompositeName;

static QHash<QByteArray, QByteArray> cppToId;

/* Takes a C++ type name, such as Qt::LayoutDirection or QString and
   maps it to how it should appear in the description file.

   These names need to be unique globally, so we don't change the C++ symbol's
   name much. It is mostly used to for explicit translations such as
   QString->string and translations for extended QML objects.
*/
QByteArray convertToId(const QByteArray &cppName)
{
    return cppToId.value(cppName, cppName);
}

QByteArray convertToId(const QMetaObject *mo)
{
    QByteArray className(mo->className());
    if (!className.isEmpty())
        return convertToId(className);

    // likely a metaobject generated for an extended qml object
    if (mo->superClass()) {
        className = convertToId(mo->superClass());
        className.append("_extended");
        return className;
    }

    static QHash<const QMetaObject *, QByteArray> generatedNames;
    className = generatedNames.value(mo);
    if (!className.isEmpty())
        return className;

    std::cerr << "Found a QMetaObject without a className, generating a random name" << std::endl;
    className = QByteArray("error-unknown-name-");
    className.append(QByteArray::number(generatedNames.size()));
    generatedNames.insert(mo, className);
    return className;
}


// Collect all metaobjects for types registered with qmlRegisterType() without parameters
void collectReachableMetaObjectsWithoutQmlName( QSet<const QMetaObject *>& metas ) {
    Q_FOREACH (const QQmlType *ty, QQmlMetaType::qmlAllTypes()) {
        if ( ! metas.contains(ty->metaObject()) ) {
            if (!ty->isComposite()) {
                collectReachableMetaObjects(ty, &metas);
            } else {
                qmlTypesByCompositeName[ty->elementName()] = ty;
            }
       }
    }
}

QSet<const QMetaObject *> collectReachableMetaObjects(QQmlEngine *engine,
                                                      QSet<const QMetaObject *> &noncreatables,
                                                      QSet<const QMetaObject *> &singletons,
                                                      const QList<QQmlType *> &skip = QList<QQmlType *>())
{
    QSet<const QMetaObject *> metas;
    metas.insert(FriendlyQObject::qtMeta());

    QHash<QByteArray, QSet<QByteArray> > extensions;
    Q_FOREACH (const QQmlType *ty, QQmlMetaType::qmlTypes()) {
        //if(!QString(ty->typeName()).startsWith("Q"))
        //std::cout << "ty " << qPrintable(ty->typeName()) << "\n";
        if (!ty->isCreatable())
            noncreatables.insert(ty->metaObject());
        if (ty->isSingleton())
            singletons.insert(ty->metaObject());
        if (!ty->isComposite()) {
            qmlTypesByCppName[ty->metaObject()->className()].insert(ty);
            if (ty->isExtendedType())
                extensions[ty->typeName()].insert(ty->metaObject()->className());
            collectReachableMetaObjects(ty, &metas);
        } else {
            qmlTypesByCompositeName[ty->elementName()] = ty;
        }
    }

    // Adjust exports of the base object if there are extensions.
    // For each export of a base object there can be a single extension object overriding it.
    // Example: QDeclarativeGraphicsWidget overrides the QtQuick/QGraphicsWidget export
    //          of QGraphicsWidget.
    Q_FOREACH (const QByteArray &baseCpp, extensions.keys()) {
        QSet<const QQmlType *> baseExports = qmlTypesByCppName.value(baseCpp);

        const QSet<QByteArray> extensionCppNames = extensions.value(baseCpp);
        Q_FOREACH (const QByteArray &extensionCppName, extensionCppNames) {
            const QSet<const QQmlType *> extensionExports = qmlTypesByCppName.value(extensionCppName);

            // remove extension exports from base imports
            // unfortunately the QQmlType pointers don't match, so can't use QSet::subtract
            QSet<const QQmlType *> newBaseExports;
            Q_FOREACH (const QQmlType *baseExport, baseExports) {
                bool match = false;
                Q_FOREACH (const QQmlType *extensionExport, extensionExports) {
                    if (baseExport->qmlTypeName() == extensionExport->qmlTypeName()
                            && baseExport->majorVersion() == extensionExport->majorVersion()
                            && baseExport->minorVersion() == extensionExport->minorVersion()) {
                        match = true;
                        break;
                    }
                }
                if (!match)
                    newBaseExports.insert(baseExport);
            }
            baseExports = newBaseExports;
        }
        qmlTypesByCppName[baseCpp] = baseExports;
    }

    if (creatable) {
        // find even more QMetaObjects by instantiating QML types and running
        // over the instances
        Q_FOREACH (QQmlType *ty, QQmlMetaType::qmlTypes()) {
            if (skip.contains(ty))
                continue;
            if (ty->isExtendedType())
                continue;
            if (!ty->isCreatable())
                continue;
            if (ty->typeName() == "QQmlComponent")
                continue;

            QString tyName = ty->qmlTypeName();
            tyName = tyName.mid(tyName.lastIndexOf(QLatin1Char('/')) + 1);
            if (tyName.isEmpty())
                continue;

            inObjectInstantiation = tyName;
            QObject *object = 0;

            if (ty->isSingleton()) {
                QQmlType::SingletonInstanceInfo *siinfo = ty->singletonInstanceInfo();
                if (!siinfo) {
                    std::cerr << "Internal error, " << qPrintable(tyName)
                              << "(" << qPrintable( QString::fromUtf8(ty->typeName()) ) << ")"
                              << " is singleton, but has no singletonInstanceInfo" << std::endl;
                    continue;
                }
                if (siinfo->qobjectCallback) {
                    if (verbose)
                        std::cerr << "Trying to get singleton for " << qPrintable(tyName)
                                  << " (" << qPrintable( siinfo->typeName )  << ")" << std::endl;
                    siinfo->init(engine);
                    collectReachableMetaObjects(object, &metas);
                    object = siinfo->qobjectApi(engine);
                } else {
                    inObjectInstantiation.clear();
                    continue; // we don't handle QJSValue singleton types.
                }
            } else {
                if (verbose)
                    std::cerr << "Trying to create object " << qPrintable( tyName )
                              << " (" << qPrintable( QString::fromUtf8(ty->typeName()) )  << ")" << std::endl;
                object = ty->create();
            }

            inObjectInstantiation.clear();

            if (object) {
                if (verbose)
                    std::cerr << "Got " << qPrintable( tyName )
                              << " (" << qPrintable( QString::fromUtf8(ty->typeName()) ) << ")" << std::endl;
                collectReachableMetaObjects(object, &metas);
                object->deleteLater();
            } else {
                std::cerr << "Could not create" << qPrintable(tyName) << std::endl;
            }
        }
    }

    collectReachableMetaObjectsWithoutQmlName(metas);

    return metas;
}

class KnownAttributes {
    QHash<QByteArray, int> m_properties;
    QHash<QByteArray, QHash<int, int> > m_methods;
public:
    bool knownMethod(const QByteArray &name, int nArgs, int revision)
    {
        if (m_methods.contains(name)) {
            QHash<int, int> overloads = m_methods.value(name);
            if (overloads.contains(nArgs) && overloads.value(nArgs) <= revision)
                return true;
        }
        m_methods[name][nArgs] = revision;
        return false;
    }

    bool knownProperty(const QByteArray &name, int revision)
    {
        if (m_properties.contains(name) && m_properties.value(name) <= revision)
            return true;
        m_properties[name] = revision;
        return false;
    }
};

class Dumper
{
    QString relocatableModuleUri;
    int indent;

public:
    Dumper(): indent(1) {}

    void setRelocatableModuleUri(const QString &uri)
    {
        relocatableModuleUri = uri;
    }

    const QString getExportString(QString qmlTyName, int majorVersion, int minorVersion)
    {
        if (qmlTyName.startsWith(relocatableModuleUri + QLatin1Char('/'))) {
            qmlTyName.remove(0, relocatableModuleUri.size() + 1);
        }
        if (qmlTyName.startsWith("./")) {
            qmlTyName.remove(0, 2);
        }
        if (qmlTyName.startsWith("/")) {
            qmlTyName.remove(0, 1);
        }
        const QString exportString = enquote(
                    QString("%1 %2.%3").arg(
                        qmlTyName,
                        QString::number(majorVersion),
                        QString::number(minorVersion)));
        return exportString;
    }

    void writeMetaContent(const QMetaObject *meta, KnownAttributes *knownAttributes = 0)
    {
        QSet<QString> implicitSignals;
        for (int index = meta->propertyOffset(); index < meta->propertyCount(); ++index) {
            const QMetaProperty &property = meta->property(index);
            dump(property, knownAttributes);
            if (knownAttributes)
                knownAttributes->knownMethod(QByteArray(property.name()).append("Changed"),
                                             0, property.revision());
            implicitSignals.insert(QString("%1Changed").arg(QString::fromUtf8(property.name())));
        }

        if (meta == &QObject::staticMetaObject) {
            // for QObject, hide deleteLater() and onDestroyed
            for (int index = meta->methodOffset(); index < meta->methodCount(); ++index) {
                QMetaMethod method = meta->method(index);
                QByteArray signature = method.methodSignature();
                if (signature == QByteArrayLiteral("destroyed(QObject*)")
                        || signature == QByteArrayLiteral("destroyed()")
                        || signature == QByteArrayLiteral("deleteLater()"))
                    continue;
                dump(method, implicitSignals, knownAttributes);
            }

            // and add toString(), destroy() and destroy(int)
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("toString"), 0, 0)) {
                writeStartObject(QLatin1String("Method"));
                writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("toString")));
                writeEndObject();
            }
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("destroy"), 0, 0)) {
                writeStartObject(QLatin1String("Method"));
                writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("destroy")));
                writeEndObject();
            }
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("destroy"), 1, 0)) {
                writeStartObject(QLatin1String("Method"));
                writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("destroy")));
                writeStartObject(QLatin1String("Parameter"));
                writeScriptBinding(QLatin1String("name"), enquote(QLatin1String("delay")));
                writeScriptBinding(QLatin1String("type"), enquote(QLatin1String("int")));
                writeEndObject();
                writeEndObject();
            }
        } else {
            for (int index = meta->methodOffset(); index < meta->methodCount(); ++index)
                dump(meta->method(index), implicitSignals, knownAttributes);
        }
    }

    QString getPrototypeNameForCompositeType(const QMetaObject *metaObject, QSet<QByteArray> &defaultReachableNames,
                                             QList<const QMetaObject *> *objectsToMerge)
    {
        QString prototypeName;
        if (!defaultReachableNames.contains(metaObject->className())) {
            // dynamic meta objects can break things badly
            // but extended types are usually fine
            const QMetaObjectPrivate *mop = reinterpret_cast<const QMetaObjectPrivate *>(metaObject->d.data);
            if (!(mop->flags & DynamicMetaObject) && objectsToMerge
                    && !objectsToMerge->contains(metaObject))
                objectsToMerge->append(metaObject);
            const QMetaObject *superMetaObject = metaObject->superClass();
            if (!superMetaObject)
                prototypeName = "QObject";
            else
                prototypeName = getPrototypeNameForCompositeType(
                            superMetaObject, defaultReachableNames, objectsToMerge);
        } else {
            prototypeName = convertToId(metaObject->className());
        }
        return prototypeName;
    }

    void dumpComposite(QQmlEngine *engine, const QQmlType *compositeType, QSet<QByteArray> &defaultReachableNames)
    {
        QQmlComponent e(engine, compositeType->sourceUrl());
        QObject *object = e.create();

        if (!object)
            return;

        QString qmlTyName = compositeType->qmlTypeName();
        writeStartObject(qmlTyName);

        const QMetaObject *mainMeta = object->metaObject();

        QList<const QMetaObject *> objectsToMerge;
        KnownAttributes knownAttributes;
        // Get C++ base class name for the composite type
        QString prototypeName = getPrototypeNameForCompositeType(mainMeta, defaultReachableNames,
                                                                 &objectsToMerge);
        writeScriptBinding(enquote("prototype"), enquote(prototypeName));

        const QString exportString = getExportString(qmlTyName, compositeType->majorVersion(), compositeType->minorVersion());
        writeArrayBinding(enquote("exports"), QStringList() << exportString);
        writeArrayBinding(enquote("exportMetaObjectRevisions"), QStringList() << QString::number(compositeType->minorVersion()));
        writeBooleanBinding(enquote("isComposite"), true);

        if (compositeType->isSingleton()) {
            writeBooleanBinding(enquote("isCreatable"), false);
            writeBooleanBinding(enquote("isSingleton"), true);
        }

        for (int index = mainMeta->classInfoCount() - 1 ; index >= 0 ; --index) {
            QMetaClassInfo classInfo = mainMeta->classInfo(index);
            if (QLatin1String(classInfo.name()) == QLatin1String("DefaultProperty")) {
                writeScriptBinding(enquote("defaultProperty"), enquote(classInfo.value()));
                break;
            }
        }

        Q_FOREACH (const QMetaObject *meta, objectsToMerge)
            writeMetaContent(meta, &knownAttributes);

        writeEndObject();
    }

    void writeIndent()
    {
        for(int i = 0; i < indent; i++)
            std::cout << "    ";
    }

    void writeStartObject(const QString& type)
    {
        writeIndent();
        std::cout << qPrintable(QString("%1: {\n").arg(enquote(type)));
        indent++;
    }

    void writeEndObject()
    {
        indent--;
        writeIndent();
        std::cout << "},\n";
    }

    void writeScriptBinding(const QString& name, const QString& value)
    {
        writeIndent();
        std::cout << qPrintable(QString("%1: %2,\n").arg(name).arg(value));
    }

    void writeBooleanBinding(const QString &name, bool value)
    {
         writeScriptBinding(name, value ? "true" : "false");
    }

    void writeArrayBinding(const QString &name, const QStringList &elements)
    {
         writeScriptBinding(name, QString("[ %1 ]").arg(elements.join(", ")));
    }

    void writeScriptObjectLiteralBinding(const QString &name, const QList<QPair<QString, QString> > &keyValue)
    {
        writeIndent();
        std::cout << qPrintable(QString("%1: [\n").arg(enquote(name)));
        indent++;
        for (int i = 0; i < keyValue.size(); ++i) {
             const QString key = keyValue.at(i).first;
             const QString value = keyValue.at(i).second;
             writeScriptBinding(key, value);
        }
        indent--;
        writeIndent();
        std::cout << "]\n";
    }

    void dump(const QMetaObject *meta, bool isUncreatable, bool isSingleton)
    {
        QStringList exportStrings;
        QSet<const QQmlType *> qmlTypes = qmlTypesByCppName.value(meta->className());
        if (!qmlTypes.isEmpty()) {
            bool foreignNamespace = false;
            QHash<QString, const QQmlType *> exports;

            Q_FOREACH (const QQmlType *qmlTy, qmlTypes) {
                const QString exportString = getExportString(qmlTy->qmlTypeName(), qmlTy->majorVersion(), qmlTy->minorVersion());
                if (exportString.contains("/"))
                    foreignNamespace = true;
                exports.insert( exportString, qmlTy);
            }

            // Ignore classes from different namespaces
            if (foreignNamespace)
                return;

            // ensure exports are sorted and don't change order when the plugin is dumped again
            exportStrings = exports.keys();
            std::sort(exportStrings.begin(), exportStrings.end());
        }

        QByteArray id = convertToId(meta);
        writeStartObject(id);

        for (int index = meta->classInfoCount() - 1 ; index >= 0 ; --index) {
            QMetaClassInfo classInfo = meta->classInfo(index);
            if (QLatin1String(classInfo.name()) == QLatin1String("DefaultProperty")) {
                writeScriptBinding(enquote("defaultProperty"), enquote(classInfo.value()));
                break;
            }
        }

        if (meta->superClass())
            writeScriptBinding(enquote("prototype"), enquote(convertToId(meta->superClass())));

        if (!qmlTypes.isEmpty()) {
            writeArrayBinding(enquote("exports"), exportStrings);

            if (isUncreatable)
                writeBooleanBinding(enquote("isCreatable"), false);
            if (isSingleton)
                writeBooleanBinding(enquote("isSingleton"), true);

            if (const QMetaObject *attachedType = (*qmlTypes.begin())->attachedPropertiesType()) {
                // Can happen when a type is registered that returns itself as attachedPropertiesType()
                // because there is no creatable type to attach to.
                if (attachedType != meta) {
                    writeScriptBinding(enquote("attachedType"), enquote(
                                       convertToId(attachedType)));
                }
            }
        }

        for (int index = meta->enumeratorOffset(); index < meta->enumeratorCount(); ++index)
            dump(meta->enumerator(index));

        writeMetaContent(meta);

        writeEndObject();
    }

    void writeEasingCurve()
    {
        writeStartObject(enquote("QEasingCurve"));
        writeScriptBinding(enquote("prototype"), enquote("QQmlEasingValueType"));
        writeEndObject();
    }

private:
    static QString enquote(const QString &string)
    {
        return QString("\"%1\"").arg(string);
    }

    /* Removes pointer and list annotations from a type name, returning
       what was removed in isList and isPointer
    */
    static void removePointerAndList(QByteArray *typeName, bool *isList, bool *isPointer)
    {
        static QByteArray declListPrefix = "QQmlListProperty<";

        if (typeName->endsWith('*')) {
            *isPointer = true;
            typeName->truncate(typeName->length() - 1);
            removePointerAndList(typeName, isList, isPointer);
        } else if (typeName->startsWith(declListPrefix)) {
            *isList = true;
            typeName->truncate(typeName->length() - 1); // get rid of the suffix '>'
            *typeName = typeName->mid(declListPrefix.size());
            removePointerAndList(typeName, isList, isPointer);
        }

        *typeName = convertToId(*typeName);
    }

    void writeTypeProperties(QByteArray typeName, bool isWritable)
    {
        bool isList = false, isPointer = false;
        removePointerAndList(&typeName, &isList, &isPointer);

        // Strip internal _QMLTYPE_xy suffix
        QString normalizedType = QString(typeName).split("_")[0];
        writeScriptBinding(enquote("type"), enquote(normalizedType));

        if (isList)
            writeScriptBinding(enquote("isList"), "true");
        if (!isWritable)
            writeScriptBinding(enquote("isReadonly"), "true");
        if (isPointer)
            writeScriptBinding(enquote("isPointer"), "true");
    }

    void dump(const QMetaProperty &prop, KnownAttributes *knownAttributes = 0)
    {
        int revision = prop.revision();
        QByteArray propName = prop.name();
        if (knownAttributes && knownAttributes->knownProperty(propName, revision))
            return;
        writeStartObject(prop.name());
        if (revision)
            writeScriptBinding(enquote("revision"), QString::number(revision));
        writeTypeProperties(prop.typeName(), prop.isWritable());

        writeEndObject();
    }

    void dump(const QMetaMethod &meth, const QSet<QString> &implicitSignals,
              KnownAttributes *knownAttributes = 0)
    {
        if (meth.methodType() == QMetaMethod::Signal) {
            if (meth.access() != QMetaMethod::Public)
                return; // nothing to do.
        } else if (meth.access() != QMetaMethod::Public) {
            return; // nothing to do.
        }

        QByteArray name = meth.name();
        const QString typeName = convertToId(meth.typeName());

        if (implicitSignals.contains(name)
                && !meth.revision()
                && meth.methodType() == QMetaMethod::Signal
                && meth.parameterNames().isEmpty()
                && typeName == QLatin1String("void")) {
            // don't mention implicit signals
            return;
        }

        int revision = meth.revision();
        if (knownAttributes && knownAttributes->knownMethod(name, meth.parameterNames().size(), revision))
            return;
        writeStartObject(name);
        if (meth.methodType() == QMetaMethod::Signal)
            writeScriptBinding(enquote("type"), enquote("Signal"));
        else
            writeScriptBinding(enquote("type"), enquote("Method"));

        if (revision)
            writeScriptBinding(enquote("revision"), QString::number(revision));

        if (typeName != QLatin1String("void"))
            writeScriptBinding(enquote("returns"), enquote(typeName));

        for (int i = 0; i < meth.parameterTypes().size(); ++i) {
            QByteArray argName = meth.parameterNames().at(i);
            writeStartObject(argName);
            writeTypeProperties(meth.parameterTypes().at(i), true);
            writeEndObject();
        }

        writeEndObject();
    }

    void dump(const QMetaEnum &e)
    {
        writeStartObject(e.name());
        writeScriptBinding(enquote("type"), enquote("Enum"));

        QList<QPair<QString, QString> > namesValues;
        for (int index = 0; index < e.keyCount(); ++index) {
            namesValues.append(qMakePair(enquote(QString::fromUtf8(e.key(index))), QString::number(e.value(index))));
        }

        writeScriptObjectLiteralBinding("values", namesValues);
        writeEndObject();
    }
};

enum ExitCode {
    EXIT_INVALIDARGUMENTS = 1,
    EXIT_SEGV = 2,
    EXIT_IMPORTERROR = 3
};

#ifdef Q_OS_UNIX
void sigSegvHandler(int) {
    fprintf(stderr, "Error: SEGV\n");
    if (!currentProperty.isEmpty())
        fprintf(stderr, "While processing the property '%s', which probably has uninitialized data.\n", currentProperty.toLatin1().constData());
    if (!inObjectInstantiation.isEmpty())
        fprintf(stderr, "While instantiating the object '%s'.\n", inObjectInstantiation.toLatin1().constData());
    exit(EXIT_SEGV);
}
#endif

void printUsage(const QString &appName)
{
    std::cerr << qPrintable(QString(
                                 "Usage: %1 [-v] [-noinstantiate] [-defaultplatform] [-[non]relocatable] module.uri version [module/import/path]\n"
                                 "       %1 [-v] [-noinstantiate] -path path/to/qmldir/directory [version]\n"
                                 "Example: %1 Qt.labs.folderlistmodel 2.0 /home/user/dev/qt-install/imports").arg(
                                 appName)) << std::endl;
}

inline std::wostream &operator<<(std::wostream &str, const QString &s)
{
    str << s.toStdWString();
    return str;
}

void printDebugMessage(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    std::wcerr << msg << std::endl;
    // In case of QtFatalMsg the calling code will abort() when appropriate.
}

int main(int argc, char *argv[])
{
    // The default message handler might not print to console on some systems. Enforce this.
    qInstallMessageHandler(printDebugMessage);
#ifdef Q_OS_UNIX
    // qmldump may crash, but we don't want any crash handlers to pop up
    // therefore we intercept the segfault and just exit() ourselves
    struct sigaction sigAction;

    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_handler = &sigSegvHandler;
    sigAction.sa_flags   = 0;

    sigaction(SIGSEGV, &sigAction, 0);
#endif

    // don't require a window manager even though we're a QGuiApplication
    bool requireWindowManager = false;
    for (int index = 1; index < argc; ++index) {
        if (QString::fromLocal8Bit(argv[index]) == "--defaultplatform"
                || QString::fromLocal8Bit(argv[index]) == "-defaultplatform") {
            requireWindowManager = true;
            break;
        }
    }

    if (!requireWindowManager)
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("minimal"));

    QGuiApplication app(argc, argv);
    const QStringList args = app.arguments();
    const QString appName = QFileInfo(app.applicationFilePath()).baseName();
    if (args.size() < 2) {
        printUsage(appName);
        return EXIT_INVALIDARGUMENTS;
    }

    QString pluginImportUri;
    QString pluginImportVersion;
    bool relocatable = true;
    enum Action { Uri, Path };
    Action action = Uri;
    {
        QStringList positionalArgs;
        Q_FOREACH (const QString &arg, args) {
            if (!arg.startsWith(QLatin1Char('-'))) {
                positionalArgs.append(arg);
                continue;
            }

            if (arg == QLatin1String("--notrelocatable")
                    || arg == QLatin1String("-notrelocatable")
                    || arg == QLatin1String("--nonrelocatable")
                    || arg == QLatin1String("-nonrelocatable")) {
                relocatable = false;
            } else if (arg == QLatin1String("--relocatable")
                        || arg == QLatin1String("-relocatable")) {
                relocatable = true;
            } else if (arg == QLatin1String("--noinstantiate")
                       || arg == QLatin1String("-noinstantiate")) {
                creatable = false;
            } else if (arg == QLatin1String("--path")
                       || arg == QLatin1String("-path")) {
                action = Path;
            } else if (arg == QLatin1String("-v")) {
                verbose = true;
            } else if (arg == QLatin1String("--defaultplatform")
                       || arg == QLatin1String("-defaultplatform")) {
                continue;
            } else {
                std::cerr << "Invalid argument: " << qPrintable(arg) << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
        }

        if (action == Uri) {
            if (positionalArgs.size() != 3 && positionalArgs.size() != 4) {
                std::cerr << "Incorrect number of positional arguments" << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
            pluginImportUri = positionalArgs[1];
            pluginImportVersion = positionalArgs[2];
            if (positionalArgs.size() >= 4)
                pluginImportPath = positionalArgs[3];
        } else if (action == Path) {
            if (positionalArgs.size() != 2 && positionalArgs.size() != 3) {
                std::cerr << "Incorrect number of positional arguments" << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
            pluginImportPath = QDir::fromNativeSeparators(positionalArgs[1]);
            if (positionalArgs.size() == 3)
                pluginImportVersion = positionalArgs[2];
        }
    }

    QQmlEngine engine;
    if (!pluginImportPath.isEmpty()) {
        QDir cur = QDir::current();
        cur.cd(pluginImportPath);
        pluginImportPath = cur.canonicalPath();
        QDir::setCurrent(pluginImportPath);
        engine.addImportPath(pluginImportPath);
    }

    // load the QtQuick 2 plugin
    {
        QByteArray code("import QtQuick 2.0\nQtObject {}");
        QQmlComponent c(&engine);
        c.setData(code, QUrl::fromLocalFile(pluginImportPath + "/loadqtquick2.qml"));
        c.create();
        if (!c.errors().isEmpty()) {
            Q_FOREACH (const QQmlError &error, c.errors())
                std::cerr << qPrintable( error.toString() ) << std::endl;
            return EXIT_IMPORTERROR;
        }
    }

    // find all QMetaObjects reachable from the builtin module
    QSet<const QMetaObject *> uncreatableMetas;
    QSet<const QMetaObject *> singletonMetas;
    QSet<const QMetaObject *> defaultReachable = collectReachableMetaObjects(&engine, uncreatableMetas, singletonMetas);
    QList<QQmlType *> defaultTypes = QQmlMetaType::qmlTypes();
    /* Q_FOREACH(QQmlType* t, defaultTypes){
        std::cout << "t " << qPrintable(t->typeName()) << "\n";
    } */

    // add some otherwise unreachable QMetaObjects
    defaultReachable.insert(&QQuickMouseEvent::staticMetaObject);
    // QQuickKeyEvent, QQuickPinchEvent, QQuickDropEvent are not exported
    QSet<QByteArray> defaultReachableNames;

    // this will hold the meta objects we want to dump information of
    QSet<const QMetaObject *> metas;

    // find a valid QtQuick import
    QByteArray importCode;
    QQmlType *qtObjectType = QQmlMetaType::qmlType(&QObject::staticMetaObject);
    if (!qtObjectType) {
        std::cerr << "Could not find QtObject type" << std::endl;
        importCode = QByteArray("import QtQuick 2.0\n");
    } else {
        QString module = qtObjectType->qmlTypeName();
        module = module.mid(0, module.lastIndexOf(QLatin1Char('/')));
        importCode = QString("import %1 %2.%3\n").arg(module,
                                                      QString::number(qtObjectType->majorVersion()),
                                                      QString::number(qtObjectType->minorVersion())).toUtf8();
    }

    // find all QMetaObjects reachable when the specified module is imported
    if (action != Path) {
        importCode += QString("import %0 %1\n").arg(pluginImportUri, pluginImportVersion).toLatin1();
    } else {
        // pluginImportVersion can be empty
        importCode += QString("import \".\" %2\n").arg(pluginImportVersion).toLatin1();
    }

    // Create a component with all QML types to add them to the type system
    QByteArray code = importCode;
    code += "Column {\n";

    QString pluginModulePath(pluginImportPath + "/" + pluginImportUri.replace(".", "/"));
    QFile f(pluginModulePath + "/qmldir");
    f.open(QIODevice::ReadOnly);
    QQmlDirParser p;
    p.parse(f.readAll());
    if (p.hasError()) {
        std::cerr << "Failed to read " << qPrintable(f.fileName()) << std::endl;
        Q_FOREACH (const QQmlError &error, p.errors("qmldir"))
            std::cerr << qPrintable( error.toString() ) << std::endl;
        return EXIT_IMPORTERROR;
    }

    Q_FOREACH(QQmlDirParser::Component c, p.components()) {
        if (c.internal)
            continue;
        code += QString("%1 {}\n").arg(c.typeName);
    }

    code += "}";
    if (verbose)
        std::cout << "Importing QML components:" << std::endl << qPrintable(code) << std::endl;

    QQmlComponent c(&engine);
    c.setData(code, QUrl::fromLocalFile(pluginImportPath + "/typelist.qml"));
    c.create();
    if (c.errors().isEmpty()) {
        Q_FOREACH (const QQmlError &error, c.errors())
            std::cerr << qPrintable( error.toString() ) << std::endl;
        return EXIT_IMPORTERROR;
    }

    QSet<const QMetaObject *> candidates = collectReachableMetaObjects(&engine, uncreatableMetas, singletonMetas, defaultTypes);
    candidates.subtract(defaultReachable);

    // Also eliminate meta objects with the same classname.
    // This is required because extended objects seem not to share
    // a single meta object instance.
    Q_FOREACH (const QMetaObject *mo, defaultReachable)
        defaultReachableNames.insert(QByteArray(mo->className()));
    Q_FOREACH (const QMetaObject *mo, candidates) {
        if (!defaultReachableNames.contains(mo->className()))
            metas.insert(mo);
    }

    // setup static rewrites of type names
    cppToId.insert("QString", "string");
    cppToId.insert("QQmlEasingValueType::Type", "Type");

    // start dumping data
    std::cout << qPrintable(QString("{ \"%1\": \"%2\",").arg("namespace").arg(pluginImportUri.replace("/", "."))) << std::endl;

    QMap<QString, QString> scripts;
    Q_FOREACH(QQmlDirParser::Script s, p.scripts()) {
        QFile sf(pluginModulePath + "/" + s.fileName);
        if (!sf.open(QIODevice::ReadOnly)) {
            std::cerr << "Failed to read " << qPrintable(sf.fileName()) << std::endl;
            return EXIT_IMPORTERROR;
        }
        scripts.insertMulti(s.nameSpace, QString("%1 %2.%3").arg(s.nameSpace).arg(s.majorVersion).arg(s.minorVersion));
    }

    Q_FOREACH(QString nameSpace, scripts.uniqueKeys()) {
        QString script(
            "    \"%1\": {\n"
            "        type: \"script\",\n"
            "        exports: [ \"%2\" ]\n    },");
        QString exports(QStringList(scripts.values(nameSpace)).join(", "));
        std::cout << qPrintable(script.arg(nameSpace).arg(exports)) << std::endl;
    }

    // put the metaobjects into a map so they are always dumped in the same order
    QMap<QString, const QMetaObject *> nameToMeta;
    Q_FOREACH (const QMetaObject *meta, metas)
        nameToMeta.insert(convertToId(meta), meta);

    Dumper dumper;
    if (relocatable)
        dumper.setRelocatableModuleUri(pluginImportUri);
    Q_FOREACH (const QMetaObject *meta, nameToMeta) {
        dumper.dump(meta, uncreatableMetas.contains(meta), singletonMetas.contains(meta));
    }
    Q_FOREACH (const QQmlType *compositeType, qmlTypesByCompositeName)
        dumper.dumpComposite(&engine, compositeType, defaultReachableNames);

    // define QEasingCurve as an extension of QQmlEasingValueType, this way
    // properties using the QEasingCurve type get useful type information.
    if (pluginImportUri.isEmpty())
        dumper.writeEasingCurve();

    std::cout << "}";

    // workaround to avoid crashes on exit
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(0);
    QObject::connect(&timer, SIGNAL(timeout()), &app, SLOT(quit()));
    timer.start();

    return app.exec();
}
