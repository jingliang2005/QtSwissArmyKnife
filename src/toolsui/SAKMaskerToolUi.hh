/******************************************************************************
 * Copyright 2023 Qsaker(wuuhaii@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 *****************************************************************************/
#ifndef SAKMASKERTOOLUI_HH
#define SAKMASKERTOOLUI_HH

#include "SAKBaseToolUi.hh"

namespace Ui {
class SAKMaskerToolUi;
}

class SAKMaskerTool;
class SAKMaskerToolUi : public SAKBaseToolUi
{
    Q_OBJECT
public:
    SAKMaskerToolUi(QWidget *parent = Q_NULLPTR);
    ~SAKMaskerToolUi();
    void setToolName(const QString &name);
    void setupMasker(SAKMaskerTool *tool);

    virtual void setupSettingsGroup(const QString &group) final;

private:
    SAKMaskerTool *mTool{nullptr};

private:
    Ui::SAKMaskerToolUi *ui;
};

#endif // SAKMASKERTOOLUI_HH
