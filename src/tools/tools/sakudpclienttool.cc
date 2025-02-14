﻿/*******************************************************************************
 * Copyright 2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#include <QHostAddress>

#include "sakinterface.h"
#include "sakcompatibility.h"
#include "sakudpclienttool.h"

SAKUdpClientTool::SAKUdpClientTool(QObject *parent)
    : SAKSocketClientTool{"sak.udpclienttool", parent}
{

}

bool SAKUdpClientTool::initialize(QString &errStr)
{
    mUdpSocket = new QUdpSocket();
    if (mSpecifyClientIpPort) {
        if (!mUdpSocket->bind(QHostAddress(mClientIp), mClientPort)) {
            errStr = mUdpSocket->errorString();
            outputMessage(QtWarningMsg, errStr);
            return false;
        }
    } else {
        if (!mUdpSocket->bind()) {
            errStr = mUdpSocket->errorString();
            outputMessage(QtWarningMsg, errStr);
            return false;
        }
    }

    QString ip = mUdpSocket->localAddress().toString();
    int port = mUdpSocket->localPort();
    QString info = QString("%1:%2").arg(ip).arg(port);
    mBindingIpPort = info;
    outputMessage(QtInfoMsg, info);
    emit bindingIpPortChanged();

    connect(mUdpSocket, &QUdpSocket::readyRead,
            mUdpSocket, [=](){readBytes();});
    connect(mUdpSocket, SAK_SOCKET_ERROR,  this, [=](){
        emit errorOccured(mUdpSocket->errorString());
    });

    return true;
}

void SAKUdpClientTool::writeBytes(const QByteArray &bytes,
                                  const QVariant &context)
{
    if (mServerIp.isEmpty()) {
        return;
    }

    qint64 ret = mUdpSocket->writeDatagram(bytes,
                                           QHostAddress(mServerIp),
                                           mServerPort);
    if (ret == -1) {
        QString ipport = mServerIp + ":" + QString::number(mServerPort);
        QString str = mUdpSocket->errorString();
        QString info = QString("write bytes to %1 error: %2").arg(ipport, str);
        outputMessage(QtWarningMsg, info);
    } else {
        QByteArray ba = SAKInterface::arrayToHex(bytes, ' ');
        QString hex = QString::fromLatin1(ba);
        QString portStr = QString::number(mServerPort);
        QString serverInfo = QString("%1:%2").arg(mServerIp, portStr);
        QString info = mBindingIpPort + "->" + serverInfo + ":" + hex;
        outputMessage(QtInfoMsg, info);
        emit bytesInputted(bytes, context);
    }
}

void SAKUdpClientTool::uninitialize()
{
    mUdpSocket->deleteLater();
    mUdpSocket = nullptr;
}

void SAKUdpClientTool::readBytes()
{
    while (mUdpSocket->hasPendingDatagrams()) {
        auto len = mUdpSocket->pendingDatagramSize();
        if (len == -1) {
            break;
        }

        QByteArray bytes(len, 0);
        QHostAddress address;
        quint16 port;
        qint64 ret = mUdpSocket->readDatagram(bytes.data(),
                                              bytes.length(),
                                              &address, &port);
        if (ret == -1) {
            outputMessage(QtWarningMsg, mUdpSocket->errorString());
        } else {
            QByteArray ba = SAKInterface::arrayToHex(bytes, ' ');
            QString hex = QString::fromLatin1(ba);
            QString portStr = address.toString();
            QString serverInfo = address.toString() + ":" + portStr;
            QString info = mBindingIpPort + "<-" + serverInfo + ":" + hex;
            outputMessage(QtInfoMsg, info);
            emit bytesOutputted(bytes, QVariant());
        }
    }
}
