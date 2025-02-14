/*******************************************************************************
 * Copyright 2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#ifndef SAKPORTNAMECOMBOBOX_H
#define SAKPORTNAMECOMBOBOX_H

#include "sakcombobox.h"

class SAKPortNameComboBox : public SAKComboBox
{
    Q_OBJECT
public:
    SAKPortNameComboBox(QWidget *parent = nullptr);

private:
    void refresh();
};

#endif // SAKPORTNAMECOMBOBOX_H
