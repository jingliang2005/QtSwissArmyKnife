﻿/*
 * Copyright 2020 Qter(qsak@foxmail.com). All rights reserved.
 *
 * The file is encoding with utf-8 (with BOM). It is a part of QtSwissArmyKnife
 * project(https://www.qsak.pro). The project is an open source project. You can
 * get the source of the project from: "https://github.com/qsak/QtSwissArmyKnife"
 * or "https://gitee.com/qsak/QtSwissArmyKnife". Also, you can join in the QQ
 * group which number is 952218522 to have a communication.
 */

#include "SAKInputCrcSettingsDialog.hh"
#include "ui_SAKInputCrcSettingsDialog.h"

SAKInputCrcSettingsDialog::SAKInputCrcSettingsDialog(QWidget *parent)
    :QDialog(parent)
    ,ui(new Ui::SAKInputCrcSettingsDialog)
{
    ui->setupUi(this);
    bigEndianCheckBox = ui->bigEndianCheckBox;
    startSpinBox = ui->startSpinBox;
    endSpinBox = ui->endSpinBox;
    setModal(true);
    setWindowTitle(tr("CRC参数设置"));
}

SAKInputCrcSettingsDialog::~SAKInputCrcSettingsDialog()
{
    delete ui;
}

SAKInputCrcSettingsDialog::ParameterContext SAKInputCrcSettingsDialog::parametersContext()
{
    SAKInputCrcSettingsDialog::ParameterContext ctx;
    parametersContextMutex.lock();
    ctx.endByte = _parametersContext.endByte;
    ctx.startByte = _parametersContext.startByte;
    ctx.isBigEndianCRC = _parametersContext.isBigEndianCRC;
    parametersContextMutex.unlock();
    return ctx;
}

void SAKInputCrcSettingsDialog::on_bigEndianCheckBox_clicked()
{
    parametersContextMutex.lock();
    _parametersContext.isBigEndianCRC = bigEndianCheckBox->isChecked();
    parametersContextMutex.unlock();
}

void SAKInputCrcSettingsDialog::on_startSpinBox_valueChanged(int value)
{
    parametersContextMutex.lock();
    _parametersContext.startByte = value;
    parametersContextMutex.unlock();
}

void SAKInputCrcSettingsDialog::on_endSpinBox_valueChanged(int value)
{
    parametersContextMutex.lock();
    _parametersContext.endByte = value;
    parametersContextMutex.unlock();
}
