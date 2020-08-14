﻿/*
 * Copyright 2018-2020 Qter(qsaker@qq.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 */
#ifndef SAKTCPSERVERDEBUGPAGE_HH
#define SAKTCPSERVERDEBUGPAGE_HH

#include "SAKDebugPage.hh"

class SAKTcpServerDevice;
class SAKTcpServerDeviceController;
class SAKTcpServerDebugPage : public SAKDebugPage
{
    Q_OBJECT
public:
    SAKTcpServerDebugPage(QWidget *parent = Q_NULLPTR);
    ~SAKTcpServerDebugPage();

    /**
     * @brief controllerInstance 获取控制类实例指针
     * @return 控制类实例
     */
    SAKTcpServerDeviceController *controllerInstance();
protected:
    SAKDebugPageController *createController() final;
    SAKDebugPageDevice* createDevice() final;
    void setUiEnable(bool enable);
private:
    SAKTcpServerDeviceController *tcpServerDeviceController;
};

#endif
