﻿/*
 * Copyright (C) 2018-2019 wuuhii. All rights reserved.
 *
 * The file is encoding with utf-8 (with BOM). It is a part of QtSwissArmyKnife
 * project. The project is a open source project, you can get the source from:
 *     https://github.com/wuuhii/QtSwissArmyKnife
 *     https://gitee.com/wuuhii/QtSwissArmyKnife
 *
 * If you want to know more about the project, please join our QQ group(952218522).
 * In addition, the email address of the project author is wuuhii@outlook.com.
 */
#ifndef SAKMOREOTHERSETTINGS_HH
#define SAKMOREOTHERSETTINGS_HH

#include <QWidget>

namespace Ui {
    class SAKMoreSettingWidget;
}

class SAKDebugPage;

class SAKMoreSettingWidget:public QWidget
{
    Q_OBJECT
public:
    SAKMoreSettingWidget(SAKDebugPage *debugPage, QWidget *parent = nullptr);
    ~SAKMoreSettingWidget();
private:
    SAKDebugPage *_debugPage;
    Ui::SAKMoreSettingWidget *ui;
};

#endif
