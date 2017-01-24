/*
 * Copyright 2016 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Arthur Mello <arthur.mello@canonical.com>
 */

#include "mockservice_adaptor.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtDBus/QDBusConnection>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtQuick/QQuickItem>
#include <UbuntuToolkit/ubuntutoolkitmodule.h>
#include <UbuntuToolkit/private/uccontenthub_p.h>

#include "uctestcase.h"

UT_USE_NAMESPACE

class MockContentService : public QObject
{
    Q_OBJECT

public:
    MockContentService() {}
    ~MockContentService() {}

public Q_SLOTS:
    void RequestPasteByAppId(const QString &appId)
    {
        Q_UNUSED(appId);
        Q_EMIT PasteRequested();
    }

    QStringList PasteFormats()
    {
        QStringList formats;
        formats << "text/plain" << "text/html" << "image/jpeg";
        return formats;
    }

Q_SIGNALS:
    void PasteSelected(const QString&, const QByteArray&, bool);
    void PasteboardChanged();

    void PasteRequested();
};

class tst_UCContentHub : public QObject
{
    Q_OBJECT

public:
    tst_UCContentHub() {}

private:
    MockContentService *mockService;
    UCContentHub *contentHub;
    QSignalSpy *pasteRequestedSpy;
    QSignalSpy *pasteSelectedSpy;

    const int waitTimeout = 5000;

    const QString dummyAppId = "DummyAppId";

    const QString sampleText = "TextData";
    const QString sampleHtml = "<html><body><p>HtmlTest</p></body></html>";

    // Following serialize code is the same as used by content-hub
    QByteArray serializeMimeData(const QMimeData &mimeData)
    {
        /*
          Data format:
            number of mime types      (sizeof(int))
            data layout               ((4 * sizeof(int)) * number of mime types)
              mime type string offset (sizeof(int))
              mime type string size   (sizeof(int))
              data offset             (sizeof(int))
              data size               (sizeof(int))
            data                      (n bytes)
        */

        const int maxFormatsCount = 16;
        const int maxBufferSize = 4 * 1024 * 1024;  // 4 Mb

        const QStringList formats = mimeData.formats();
        const int formatCount = qMin(formats.size(), maxFormatsCount);
        const int headerSize = sizeof(int) + (formatCount * 4 * sizeof(int));
        int bufferSize = headerSize;

        for (int i = 0; i < formatCount; i++)
            bufferSize += formats[i].size() + mimeData.data(formats[i]).size();

        QByteArray serializedMimeData;
        if (bufferSize <= maxBufferSize) {
            // Serialize data.
            serializedMimeData.resize(bufferSize);
            {
                char *buffer = serializedMimeData.data();
                int* header = reinterpret_cast<int*>(serializedMimeData.data());
                int offset = headerSize;
                header[0] = formatCount;
                for (int i = 0; i < formatCount; i++) {
                    const QByteArray data = mimeData.data(formats[i]);
                    const int formatOffset = offset;
                    const int formatSize = formats[i].size();
                    const int dataOffset = offset + formatSize;
                    const int dataSize = data.size();
                    memcpy(&buffer[formatOffset], formats[i].toLatin1().data(), formatSize);
                    memcpy(&buffer[dataOffset], data.data(), dataSize);
                    header[i*4+1] = formatOffset;
                    header[i*4+2] = formatSize;
                    header[i*4+3] = dataOffset;
                    header[i*4+4] = dataSize;
                    offset += formatSize + dataSize;
                }
            }
        }

        return serializedMimeData;
    }


private Q_SLOTS:
    void initTestCase()
    {
        mockService = new MockContentService();
        new ServiceAdaptor(mockService);
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.registerObject("/", mockService);
        connection.registerService("com.ubuntu.content.dbus.Service");
        pasteRequestedSpy = new QSignalSpy(mockService, SIGNAL(PasteRequested()));

        qRegisterMetaType<QQuickItem*>();
        contentHub = new UCContentHub();
        pasteSelectedSpy = new QSignalSpy(contentHub, SIGNAL(pasteSelected(QQuickItem*, const QString&)));
    }

    void cleanupTestCase()
    {
        delete pasteRequestedSpy;
        delete pasteSelectedSpy;
        delete contentHub;
    }

    void cleanup()
    {
        pasteRequestedSpy->clear();
        pasteSelectedSpy->clear();
    }

    void test_DeserializeTextMimeData()
    {
        QMimeData textMimeData;
        textMimeData.setText(sampleText);
        QMimeData *deserialized = contentHub->deserializeMimeData(serializeMimeData(textMimeData));
        QCOMPARE(deserialized->formats().size(), 1);
        QVERIFY(deserialized->hasText());
        QVERIFY(deserialized->text() == sampleText);
    }

    void test_DeserializeHtmlMimeData()
    {
        QMimeData htmlMimeData;
        htmlMimeData.setHtml(sampleHtml);
        QMimeData *deserialized = contentHub->deserializeMimeData(serializeMimeData(htmlMimeData));
        QVERIFY(deserialized->hasHtml());
        QVERIFY(deserialized->html() == sampleHtml);
    }

    void test_TextPasteSelected()
    {
        QMimeData textPaste;
        textPaste.setText(sampleHtml);
        contentHub->onPasteSelected(contentHub->getAppProfile(), serializeMimeData(textPaste), false);
        pasteSelectedSpy->wait(waitTimeout);
        QCOMPARE(pasteSelectedSpy->count(), 1);
        QList<QVariant> args = pasteSelectedSpy->takeFirst();
        QVERIFY(args.at(1).toString() == textPaste.text());
    }

    void test_HtmlPasteSelectedAsText()
    {
        QMimeData htmlPaste;
        htmlPaste.setHtml(sampleHtml);
        contentHub->onPasteSelected(contentHub->getAppProfile(), serializeMimeData(htmlPaste), false);
        pasteSelectedSpy->wait(waitTimeout);
        QCOMPARE(pasteSelectedSpy->count(), 1);
        QList<QVariant> args = pasteSelectedSpy->takeFirst();
        QVERIFY(args.at(1).toString() == htmlPaste.text());
    }

    void test_HtmlPasteSelectedAsRichText()
    {
        QMimeData htmlPaste;
        htmlPaste.setHtml(sampleHtml);
        contentHub->onPasteSelected(contentHub->getAppProfile(), serializeMimeData(htmlPaste), true);
        pasteSelectedSpy->wait(waitTimeout);
        QCOMPARE(pasteSelectedSpy->count(), 1);
        QList<QVariant> args = pasteSelectedSpy->takeFirst();
        QVERIFY(args.at(1).toString() == htmlPaste.html());
    }

    void test_PasteFromAnotherAppId()
    {
        QMimeData textPaste;
        textPaste.setText(sampleText);
        contentHub->onPasteSelected(dummyAppId, serializeMimeData(textPaste), false);
        pasteSelectedSpy->wait(waitTimeout);
        QCOMPARE(pasteSelectedSpy->count(), 0);
    }

    void test_KeyboardShortcutOnTextField()
    {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("TextFieldPaste.qml"));
        QQuickItem *textField = testCase->findItem<QQuickItem*>("textField");
        QTest::keyClick(textField->window(), Qt::Key_V, Qt::ControlModifier|Qt::ShiftModifier);
        pasteRequestedSpy->wait(waitTimeout);
        QCOMPARE(pasteRequestedSpy->count(), 1);
    }
    
    void test_KeyboardShortcutOnTextArea()
    {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("TextAreaPaste.qml"));
        QQuickItem *textArea = testCase->findItem<QQuickItem*>("textArea");
        QTest::keyClick(textArea->window(), Qt::Key_V, Qt::ControlModifier|Qt::ShiftModifier);
        pasteRequestedSpy->wait(waitTimeout);
        QCOMPARE(pasteRequestedSpy->count(), 1);
    }
};

QTEST_MAIN(tst_UCContentHub)

#include "tst_contenthub.moc"
