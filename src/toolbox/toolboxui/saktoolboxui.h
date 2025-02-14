/*******************************************************************************
 * Copyright 2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#ifndef SAKTOOLBOXUI_H
#define SAKTOOLBOXUI_H

#include <QIcon>
#include <QList>
#include <QTimer>
#include <QWidget>
#include <QLoggingCategory>

#include "saktoolbox.h"
#include "sakcommunicationtoolui.h"

namespace Ui {
class SAKToolBoxUi;
}

class SAKEmitterToolUi;
class SAKResponserToolUi;
class SAKPrestorerToolUi;
class SAKCommunicationTool;
class SAKToolBoxUiInputMenu;
class SAKToolBoxUiOutputMenu;
class SAKCommunicationToolUi;
class SAKTcpTransmitterToolUi;
class SAKUdpTransmitterToolUi;
class SAKToolBoxUiCommunicationMenu;
class SAKWebSocketTransmitterToolUi;
class SAKSerialPortTransmitterToolUi;
class SAKToolBoxUi : public QWidget
{
    Q_OBJECT
public:
    explicit SAKToolBoxUi(QWidget *parent = nullptr);
    ~SAKToolBoxUi();

    static QList<int> supportedCommuniticationTools();
    static QString communiticationToolName(int type);
    static QIcon communiticationToolIcon(int type);

    void initialize(int type);

private:
    SAKToolBox *mToolBox{nullptr};
    SAKCommunicationTool *mCommunicationTool{nullptr};
    SAKCommunicationToolUi *mCommunicationToolUi{nullptr};
    SAKEmitterToolUi *mEmitterToolUi{nullptr};
    SAKResponserToolUi *mResponserToolUi{nullptr};
    SAKPrestorerToolUi *mPrestorerToolUi{nullptr};
    SAKTcpTransmitterToolUi *mTcpTransmitterToolUi{nullptr};
    SAKUdpTransmitterToolUi *mUdpTransmitterToolUi{nullptr};
    SAKWebSocketTransmitterToolUi *mWebSocketTransmitterToolUi{nullptr};
    SAKSerialPortTransmitterToolUi *mSerialPortTransmitterToolUi{nullptr};
    QLoggingCategory mLoggingCategory{"sak.toolboxui"};
    QTimer *mCycleSendingTimer{nullptr};
    int mCommunicationToolType;
    SAKToolBoxUiCommunicationMenu *mCommunicationMenu{nullptr};
    SAKToolBoxUiInputMenu *mInputMenu{nullptr};
    SAKToolBoxUiOutputMenu *mOutputMenu{nullptr};

    struct {
        QString tabIndex;
        QString items;
        QString transmitterTabIndex;
    } mSettingsKey;

private:
    SAKCommunicationToolUi *communiticationToolUi(int type);
    void try2send();
    QString dateTimeFormat();
    void output2ui(const QByteArray &bytes, const QVariant &context, bool isRx);
    QString settingsGroup();
    QByteArray calculateCrc(const QByteArray &bytes = QByteArray(),
                            bool fixedOriginOrder = false);
    void setDefaultText();

private slots:
    void onIsWorkingChanged();
    void onBytesWritten(const QByteArray &bytes, const QVariant &context);
    void onBytesRead(const QByteArray &bytes, const QVariant &context);
    void onInputTextChanged();

private:
    void init();

    void initUi();
    void initUiCommunication();
    void initUiInput();
    void initUiOutput();

    void initSettings();
    void initSettingsCommunication();
    void initSettingsInput();
    void initSettingsOutput();

    void initSignals();
    void initSignalsCommunication();
    void initSignalsInput();
    void initSignalsOutput();

    void initTools();

private:
    Ui::SAKToolBoxUi *ui;
    void onTabWidgetCurrentChanged(int index);

    // communication
    void onPushButtonCommunicationOpenClicked();

    // input
    void onPushButtonInputSendClicked();
    void onComboBoxInputIntervelActivated();
    void onComboBoxInputFormatActivated();
    void onComboBoxInputTextActivated();

    // output
    void onCheckBoxOutputWrapClicked();
};

#endif // SAKTOOLBOXUI_H
