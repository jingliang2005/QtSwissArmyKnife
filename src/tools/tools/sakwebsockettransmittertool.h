/*******************************************************************************
 * Copyright 2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#ifndef SAKWEBSOCKETTRANSMITTERTOOL_H
#define SAKWEBSOCKETTRANSMITTERTOOL_H

#include "saksocketclienttransmittertool.h"

class SAKWebSocketTransmitterTool : public SAKSocketClientTransmitterTool
{
    Q_OBJECT
public:
    explicit SAKWebSocketTransmitterTool(QObject *parent = nullptr);
    virtual SAKCommunicationTool *createTool() override;
};

#endif // SAKWEBSOCKETTRANSMITTERTOOL_H
