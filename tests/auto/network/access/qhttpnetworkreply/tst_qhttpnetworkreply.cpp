/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include "private/qhttpnetworkconnection_p.h"

class tst_QHttpNetworkReply: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void parseHeader_data();
    void parseHeader();
};

void tst_QHttpNetworkReply::parseHeader_data()
{
    QTest::addColumn<QByteArray>("headers");
    QTest::addColumn<QStringList>("fields");
    QTest::addColumn<QStringList>("values");

    QTest::newRow("empty-field") << QByteArray("Set-Cookie: \r\n")
                                 << (QStringList() << "Set-Cookie")
                                 << (QStringList() << "");
    QTest::newRow("single-field") << QByteArray("Content-Type: text/html; charset=utf-8\r\n")
                                  << (QStringList() << "Content-Type")
                                  << (QStringList() << "text/html; charset=utf-8");
    QTest::newRow("single-field-continued") << QByteArray("Content-Type: text/html;\r\n"
                                                          " charset=utf-8\r\n")
                                            << (QStringList() << "Content-Type")
                                            << (QStringList() << "text/html; charset=utf-8");

    QTest::newRow("multi-field") << QByteArray("Content-Type: text/html; charset=utf-8\r\n"
                                               "Content-Length: 1024\r\n"
                                               "Content-Encoding: gzip\r\n")
                                 << (QStringList() << "Content-Type" << "Content-Length" << "Content-Encoding")
                                 << (QStringList() << "text/html; charset=utf-8" << "1024" << "gzip");
    QTest::newRow("multi-field-with-emtpy") << QByteArray("Content-Type: text/html; charset=utf-8\r\n"
                                                          "Content-Length: 1024\r\n"
                                                          "Set-Cookie: \r\n"
                                                          "Content-Encoding: gzip\r\n")
                                            << (QStringList() << "Content-Type" << "Content-Length" << "Set-Cookie" << "Content-Encoding")
                                            << (QStringList() << "text/html; charset=utf-8" << "1024" << "" << "gzip");

    QTest::newRow("lws-field") << QByteArray("Content-Type: text/html; charset=utf-8\r\n"
                                             "Content-Length:\r\n 1024\r\n"
                                             "Content-Encoding: gzip\r\n")
                               << (QStringList() << "Content-Type" << "Content-Length" << "Content-Encoding")
                               << (QStringList() << "text/html; charset=utf-8" << "1024" << "gzip");

    QTest::newRow("duplicated-field") << QByteArray("Vary: Accept-Language\r\n"
                                                    "Vary: Cookie\r\n"
                                                    "Vary: User-Agent\r\n")
                                      << (QStringList() << "Vary")
                                      << (QStringList() << "Accept-Language, Cookie, User-Agent");
}

void tst_QHttpNetworkReply::parseHeader()
{
    QFETCH(QByteArray, headers);
    QFETCH(QStringList, fields);
    QFETCH(QStringList, values);

    QHttpNetworkReply reply;
    reply.parseHeader(headers);
    for (int i = 0; i < fields.count(); ++i) {
        //qDebug() << "field" << fields.at(i) << "value" << reply.headerField(fields.at(i)) << "expected" << values.at(i);
        QString field = reply.headerField(fields.at(i).toLatin1());
        QCOMPARE(field, values.at(i));
    }
}

QTEST_MAIN(tst_QHttpNetworkReply)
#include "tst_qhttpnetworkreply.moc"
