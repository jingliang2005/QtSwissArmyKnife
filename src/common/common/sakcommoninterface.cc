﻿/*******************************************************************************
 * Copyright 2018-2021 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#include <QMap>
#include <QMetaEnum>
#include <QComboBox>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QStandardItemModel>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#ifdef SAK_IMPORT_MODULE_SERIALPORT
#include <QSerialPort>
#include <QSerialPortInfo>
#endif

#include "sakcommoninterface.h"
#include "sakcommoncrcinterface.h"

SAKCommonInterface::SAKCommonInterface(QObject *parent)
    :QObject (parent)
{

}

void SAKCommonInterface::setLineEditValidator(QLineEdit *lineEdit,
                                              SAKEnumValidatorType type,
                                              int maxLength)
{   
    QMap<int, QRegularExpression> regExpMap;
    regExpMap.insert(ValidatorBin,
                     QRegularExpression("([01][01][01][01][01][01][01][01][ ])*"));
    regExpMap.insert(ValidatorOtc,
                     QRegularExpression("([0-7][0-7][ ])*"));
    regExpMap.insert(ValidatorDec,
                     QRegularExpression("([0-9][0-9][ ])*"));
    regExpMap.insert(ValidatorHex,
                     QRegularExpression("([0-9a-fA-F][0-9a-fA-F][ ])*"));
    regExpMap.insert(ValidatorAscii,
                     QRegularExpression("([ -~])*"));
    regExpMap.insert(ValidatorFloat,
                     QRegularExpression("^[-+]?[0-9]*\\.?[0-9]+$"));

    if (lineEdit){
        if (lineEdit->validator()){
            delete lineEdit->validator();
        }

        if (regExpMap.contains(type)){
            if (type != ValidatorNone){
                auto regExpValidator =
                        new QRegularExpressionValidator(regExpMap.value(type),
                                                        lineEdit);
                lineEdit->setValidator(regExpValidator);
                lineEdit->setMaxLength(maxLength);
            }
        }
    }
}

#ifdef SAK_IMPORT_MODULE_SERIALPORT
void SAKCommonInterface::addSerialPortNametItemsToComboBox(QComboBox *comboBox)
{
    if (comboBox){
        comboBox->clear();
        QList<QSerialPortInfo> coms = QSerialPortInfo::availablePorts();
        QStandardItemModel *itemModel = new QStandardItemModel(comboBox);
        for(auto &var:coms){
            QStandardItem *item = new QStandardItem(var.portName());
            item->setToolTip(var.description());
            itemModel->appendRow(item);
        }

        comboBox->setModel(itemModel);
    }
}
#endif

#ifdef SAK_IMPORT_MODULE_SERIALPORT
void SAKCommonInterface::addSerialPortBaudRateItemsToComboBox(QComboBox *comboBox)
{
    if (comboBox){
        comboBox->clear();
        QList<qint32> bd = QSerialPortInfo::standardBaudRates();
        for (auto &var:bd) {
            comboBox->addItem(QString::number(var), QVariant::fromValue(var));
        }

        comboBox->setCurrentText("9600");
    }
}
#endif

#ifdef SAK_IMPORT_MODULE_SERIALPORT
void SAKCommonInterface::addSerialPortDataBitItemsToComboBox(QComboBox *comboBox)
{
    if (comboBox){
        comboBox->clear();
        comboBox->addItem("8", QVariant::fromValue(int(QSerialPort::Data8)));
        comboBox->addItem("7", QVariant::fromValue(int(QSerialPort::Data7)));
        comboBox->addItem("6", QVariant::fromValue(int(QSerialPort::Data6)));
        comboBox->addItem("5", QVariant::fromValue(int(QSerialPort::Data5)));
    }
}
#endif

#ifdef SAK_IMPORT_MODULE_SERIALPORT
void SAKCommonInterface::addSerialPortStopBitItemsToComboBox(QComboBox *comboBox)
{
    if (comboBox){
        comboBox->clear();
        comboBox->addItem("1",
                          QVariant::fromValue(int(QSerialPort::OneStop)));
#ifdef Q_OS_WINDOWS
        comboBox->addItem("1.5",
                          QVariant::fromValue(int(QSerialPort::OneAndHalfStop)));
#endif
        comboBox->addItem("2",
                          QVariant::fromValue(int(QSerialPort::TwoStop)));
    }
}
#endif

#ifdef SAK_IMPORT_MODULE_SERIALPORT
void SAKCommonInterface::addSerialPortParityItemsToComboBox(QComboBox *comboBox)
{
    if (comboBox){
        comboBox->clear();
        comboBox->addItem(tr("No"),
                          QVariant::fromValue(int(QSerialPort::NoParity)));
        comboBox->addItem(tr("Even"),
                          QVariant::fromValue(int(QSerialPort::EvenParity)));
        comboBox->addItem(tr("Odd"),
                          QVariant::fromValue(int(QSerialPort::OddParity)));
        comboBox->addItem(tr("Space"),
                          QVariant::fromValue(int(QSerialPort::SpaceParity)));
        comboBox->addItem(tr("Mark"),
                          QVariant::fromValue(int(QSerialPort::MarkParity)));
    }
}
#endif

#ifdef SAK_IMPORT_MODULE_SERIALPORT
void SAKCommonInterface::addSerialPortFlowControlItemsToComboBox(QComboBox *comboBox)
{
    if (comboBox){
        comboBox->clear();
        comboBox->addItem(tr("No"),
                          QVariant::fromValue(int(QSerialPort::NoFlowControl)));
        comboBox->addItem(tr("Hardware"),
                          QVariant::fromValue(int(QSerialPort::HardwareControl)));
        comboBox->addItem(tr("Software"),
                          QVariant::fromValue(int(QSerialPort::SoftwareControl)));
    }
}
#endif

void SAKCommonInterface::addIpItemsToComboBox(QComboBox *comboBox, bool appendHostAny)
{
    QString localHost("127.0.0.1");
    if (comboBox){
        comboBox->clear();
        comboBox->addItem(QString("::"));
        comboBox->addItem(QString("::1"));
        comboBox->addItem(QString("0.0.0.0"));
        comboBox->addItem(localHost);
        QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
        for(auto &var:addresses){
            if (var.protocol() == QAbstractSocket::IPv4Protocol) {
                if (var.toString().compare(localHost) == 0){
                    continue;
                }
                comboBox->addItem(var.toString());
            }
        }

        if (appendHostAny){
            comboBox->addItem(QString(SAK_HOST_ADDRESS_ANY));
        }
        comboBox->setCurrentText(localHost);
    }
}

void SAKCommonInterface::setComboBoxIndexFromSettings(QSettings *settings,
                                                      QString key,
                                                      QComboBox *comboBox)
{
    int index = settings->value(key).toInt();
    if (index >= 0 && index < comboBox->count()){
        comboBox->setCurrentIndex(index);
    }
}

void SAKCommonInterface::setSettingsValueFromComboBoxIndex(QSettings *settings,
                                                      QString key,
                                                      QComboBox *comboBox)
{
    int currentIndex = comboBox->currentIndex();
    settings->setValue(key, currentIndex);
}

void SAKCommonInterface::setLineEditTextFromSettings(QSettings *settings,
                                                     QString key,
                                                     QLineEdit *lineEdit)
{
    QString text = settings->value(key).toString();
    if (!text.isEmpty()) {
        lineEdit->setText(text);
    }
}

void SAKCommonInterface::setSettingsValueFromLineEditText(QSettings *settings,
                                                          QString key,
                                                          QLineEdit *lineEdit)
{
    QString value = lineEdit->text();
    settings->setValue(key, value);
}

void SAKCommonInterface::setCheckBoxValueFromSettings(QSettings *settings,
                                                      QString key,
                                                      QCheckBox *checkBox)
{
    checkBox->setChecked(settings->value(key).toBool());
}

void SAKCommonInterface::setSettingsValueFromCheckBox(QSettings *settings,
                                                      QString key,
                                                      QCheckBox *checkBox)
{
    settings->setValue(key, checkBox->isChecked());
}
