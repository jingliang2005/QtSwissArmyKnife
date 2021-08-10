﻿/****************************************************************************************
 * Copyright 2018-2021 Qter(qsaker@qq.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ***************************************************************************************/
#ifndef SAKDEBUGGERPLUGINTRANSPONDERS_HH
#define SAKDEBUGGERPLUGINTRANSPONDERS_HH

#include <QWidget>
#include <QSettings>
#include <QSqlDatabase>

namespace Ui {
    class SAKDebuggerPluginTransponders;
}

class SAKDebuggerPluginTransponders : public QWidget
{
    Q_OBJECT
public:
    SAKDebuggerPluginTransponders(QSqlDatabase *sqlDatabase,
                                  QSettings *settings,
                                  QString settingsGroup,
                                  QWidget *parent = Q_NULLPTR);
    ~SAKDebuggerPluginTransponders();


private:
    QSqlDatabase *mSqlDatabase;
    QSettings *mSettings;
    QString mSettingsGroup;


private:
    Ui::SAKDebuggerPluginTransponders *mUi;
};

#endif
