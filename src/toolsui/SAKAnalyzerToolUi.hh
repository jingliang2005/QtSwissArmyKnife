/******************************************************************************
 * Copyright 2023 Qsaker(wuuhaii@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 *****************************************************************************/
#ifndef SAKANALYZERTOOLUI_HH
#define SAKANALYZERTOOLUI_HH

#include "SAKBaseToolUi.hh"

namespace Ui {
class SAKAnalyzerToolUi;
}

class SAKAnalyzerTool;
class SAKAnalyzerToolUi : public SAKBaseToolUi
{
    Q_OBJECT
public:
    explicit SAKAnalyzerToolUi(QWidget *parent = nullptr);
    void setToolName(const QString &name);
    void setupAnalyzer(SAKAnalyzerTool *tool);

    virtual void setupSettingsGroup(const QString &group) final;

private:
    Ui::SAKAnalyzerToolUi *ui;
};

#endif // SAKANALYZERTOOLUI_HH
