/*******************************************************************************
 * Copyright 2018-2021 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#ifndef SAKCOMMONINTERFACE_H
#define SAKCOMMONINTERFACE_H

#include <QObject>
#include <QCheckBox>
#include <QSettings>
#include <QLineEdit>
#include <QComboBox>

#define microIni2CoB(settings, settingsGroup, structMember, comboBox)\
    SAKCommonInterface::setComboBoxIndexFromSettings(settings,\
    settingsGroup + QString("/") + QString(#structMember).split('.').last(),\
    comboBox)
#define microCoB2Ini(settings, settingsGroup, structMember, comboBox)\
    SAKCommonInterface::setSettingsValueFromComboBoxIndex(settings,\
    settingsGroup + QString("/") + QString(#structMember).split('.').last(),\
    comboBox)
#define microIni2LE(settings, settingsGroup, structMember, lineEdit)\
    SAKCommonInterface::setLineEditTextFromSettings(settings,\
    settingsGroup + QString("/") + QString(#structMember).split('.').last(),\
    lineEdit)
#define microLE2Ini(settings, settingsGroup, structMember, lineEdit)\
    SAKCommonInterface::setSettingsValueFromLineEditText(settings,\
    settingsGroup + QString("/") + QString(#structMember).split('.').last(),\
    lineEdit)
#define microIni2ChB(settings, settingsGroup, structMember, checkBox)\
    SAKCommonInterface::setCheckBoxValueFromSettings(settings,\
    settingsGroup + QString("/") + QString(#structMember).split('.').last(),\
    checkBox)
#define microChB2Ini(settings, settingsGroup, structMember, checkBox)\
    SAKCommonInterface::setSettingsValueFromCheckBox(settings,\
    settingsGroup + QString("/") + QString(#structMember).split('.').last(),\
    checkBox)

class SAKCommonInterface:public QObject
{
    Q_OBJECT
public:
    SAKCommonInterface(QObject *parent = Q_NULLPTR);

    enum SAKEnumValidatorType {
        ValidatorBin,
        ValidatorOtc,
        ValidatorDec,
        ValidatorHex,
        ValidatorAscii,
        ValidatorFloat,
        ValidatorNone
    };

    /**
     * @brief setLineEditValidator: Set validator to a line edit, i
     * t will delete the old validator.
     * @param lineEdit: Target line edit.
     * @param type: See ValidatorType for more information.
     */
    static void setLineEditValidator(QLineEdit *lineEdit,
                                     SAKEnumValidatorType type,
                                     int maxLength = INT_MAX);
#ifdef SAK_IMPORT_MODULE_SERIALPORT
    // Serialport interfaces
    static void addSerialPortNametItemsToComboBox(QComboBox *comboBox);
    static void addSerialPortBaudRateItemsToComboBox(QComboBox *comboBox);
    static void addSerialPortDataBitItemsToComboBox(QComboBox *comboBox);
    static void addSerialPortStopBitItemsToComboBox(QComboBox *comboBox);
    static void addSerialPortParityItemsToComboBox(QComboBox *comboBox);
    static void addSerialPortFlowControlItemsToComboBox(QComboBox *comboBox);
#endif
    static void addIpItemsToComboBox(QComboBox *comboBox, bool appendHostAny = false);
    static void setComboBoxIndexFromSettings(QSettings *settings, QString key, QComboBox *comboBox);
    static void setSettingsValueFromComboBoxIndex(QSettings *settings, QString key, QComboBox *comboBox);
    static void setLineEditTextFromSettings(QSettings *settings, QString key, QLineEdit *lineEdit);
    static void setSettingsValueFromLineEditText(QSettings *settings, QString key, QLineEdit *lineEdit);
    static void setCheckBoxValueFromSettings(QSettings *settings, QString key, QCheckBox *checkBox);
    static void setSettingsValueFromCheckBox(QSettings *settings, QString key, QCheckBox *checkBox);
};
#endif
