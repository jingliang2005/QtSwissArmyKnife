﻿/*******************************************************************************
 * Copyright 2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#include <QFile>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QStandardItemModel>

#include "sakmenu.h"
#include "saksettings.h"
#include "saktablemodeltool.h"
#include "saktablemodeltoolui.h"
#include "ui_saktablemodeltoolui.h"

SAKTableModelToolUi::SAKTableModelToolUi(const char *lg, QWidget *parent)
    : SAKBaseToolUi{parent}
    , mLoggingCategory{lg}
    , ui(new Ui::SAKTableModelToolUi)
{
    ui->setupUi(this);
    QHeaderView *vHeaderView = ui->tableView->verticalHeader();
    vHeaderView->hide();

    connect(ui->pushButtonEdit, &QPushButton::clicked,
            this, &SAKTableModelToolUi::onPushButtonEditClicked);
    connect(ui->pushButtonClear, &QPushButton::clicked,
            this, &SAKTableModelToolUi::onPushButtonClearClicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked,
            this, &SAKTableModelToolUi::onPushButtonDeleteClicked);
    connect(ui->pushButtonImport, &QPushButton::clicked,
            this, &SAKTableModelToolUi::onPushButtonImportClicked);
    connect(ui->pushButtonExport, &QPushButton::clicked,
            this, &SAKTableModelToolUi::onPushButtonExportClicked);
    connect(ui->pushButtonAppend, &QPushButton::clicked,
            this, &SAKTableModelToolUi::onPushButtonAppendClicked);

    connect(ui->tableView, &QTableView::doubleClicked,
            this, [=](const QModelIndex &index){
        Q_UNUSED(index)
        onPushButtonEditClicked();
    });
}

SAKTableModelToolUi::~SAKTableModelToolUi()
{
    delete ui;
}

void SAKTableModelToolUi::setStretchSections(QList<int>columns)
{
    QTableView *tableView = ui->tableView;
    QHeaderView *headerView = tableView->horizontalHeader();
    for (int &column : columns) {
        headerView->setSectionResizeMode(column, QHeaderView::Stretch);
    }
}

void SAKTableModelToolUi::setSectionResizeModeToStretch()
{
    QTableView *tableView = ui->tableView;
    QHeaderView *headerView = tableView->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);
}

void SAKTableModelToolUi::setColumnVisible(int column, bool visible)
{
    QTableView *tableView = ui->tableView;
    if (visible) {
        tableView->showColumn(column);
    } else {
        tableView->hideColumn(column);
    }
}

void SAKTableModelToolUi::onBaseToolUiInitialized(SAKBaseTool *tool,
                                                  const QString &settingGroup)
{
    if (!tool) {
        qCWarning(mLoggingCategory) << "The value of tool is nullptr!";
        return;
    }

    if (!tool->inherits("SAKTableModelTool")) {
        qCWarning(mLoggingCategory)
            << "The tool does not inherits SAKTableModelTool!";
        return;
    }

    mTableModelTool = qobject_cast<SAKTableModelTool*>(tool);
    if (!mTableModelTool) {
        qCWarning(mLoggingCategory)
            << "The value of mTableModelTool is nullptr!";
        return;
    }

    mTableModel = mTableModelTool->tableModel().value<QAbstractTableModel*>();
    QTableView *tableView = ui->tableView;
    QHeaderView *headerView = tableView->horizontalHeader();
    int columnCount = mTableModel->columnCount();
    QStringList headers;
    QStringList rawHeaders;
    for (int i = 0; i < columnCount; i++) {
        auto orientation = Qt::Orientation::Horizontal;
        QString str = mTableModel->headerData(i, orientation).toString();
        rawHeaders.append(str);
        str = mTableModelTool->cookHeaderString(str);
        headers.append(str);
    }

    QStandardItemModel *headerViewModel = new QStandardItemModel(headerView);

    tableView->setHorizontalHeader(headerView);
    tableView->setModel(mTableModel);

    headerViewModel->setColumnCount(headers.count());
    headerViewModel->setHorizontalHeaderLabels(headers);
    headerView->setModel(headerViewModel);
    headerView->setDefaultAlignment(Qt::AlignLeft);

    mMenu = new SAKMenu(ui->pushButtonVisible);
    ui->pushButtonVisible->setMenu(mMenu);
    auto settings = SAKSettings::instance();
    auto hideColumns = defaultHideColumns();
    for (int i = 0; i < headers.count(); i++) {
        QAction *ret = mMenu->addAction(headers.at(i));
        connect(ret, &QAction::triggered, this, [=](){
            if (ret->isChecked()) {
                tableView->showColumn(i);
            } else {
                tableView->hideColumn(i);
            }
            settings->setValue(settingGroup + "/" + rawHeaders.at(i),
                               ret->isChecked());

        });
        ret->setCheckable(true);

        QVariant var = settings->value(settingGroup + "/" + rawHeaders.at(i));
        if (var.isValid()) {
            if (!var.toBool()) {
                tableView->hideColumn(i);
            }
            ret->setChecked(var.toBool());
        } else {
            if (hideColumns.contains(i)) {
                tableView->hideColumn(i);
                ret->setChecked(false);
            } else {
                ret->setChecked(true);
            }
        }
    }

    mItemsKey = settingGroup + "/items";
    QString items = settings->value(mItemsKey).toString();
    QByteArray json = QByteArray::fromHex(items.toLatin1());
    importFromJson(QJsonDocument::fromJson(json).toJson());
}

QList<int> SAKTableModelToolUi::defaultHideColumns()
{
    QList<int> list;
    return list;
}

void SAKTableModelToolUi::afterRowEdited(int row)
{
    Q_UNUSED(row)
}

void SAKTableModelToolUi::clear()
{
    int rowCount = mTableModel->rowCount();
    mTableModel->removeRows(0, rowCount);
}

void SAKTableModelToolUi::remove(const QModelIndex &index)
{
    if (index.isValid()) {
        mTableModel->removeRow(index.row());
    }
}

void SAKTableModelToolUi::importFromJson(const QByteArray &json)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json);
    QJsonArray jsonArray = jsonDoc.array();
    for (int i = 0; i < jsonArray.count(); i++) {
        QJsonObject jsonObj = jsonArray.at(i).toObject();
        QJsonDocument jd;
        jd.setObject(jsonObj);
        QString item = QString::fromUtf8(jd.toJson());
        mTableModelTool->addItem(item);
    }
}

QByteArray SAKTableModelToolUi::exportAsJson()
{
    auto items = mTableModelTool->itemsContext();
    QJsonArray jsonArray = items.toJsonArray();
    QJsonDocument jsonDoc;
    jsonDoc.setArray(jsonArray);
    QByteArray json = jsonDoc.toJson();
    return json;
}

void SAKTableModelToolUi::edit(const QModelIndex &index)
{
    QVariant var = mTableModelTool->itemContext(index.row());
    QJsonObject jsonObj = var.toJsonObject();
    QDialog *editor = itemEditor();
    QGenericReturnArgument ret;
    QMetaObject::invokeMethod(editor,
                              "setParameters",
                              Qt::DirectConnection,
                              ret,
                              Q_ARG(QJsonObject, jsonObj));
    qCInfo(mLoggingCategory) << "the parameter of setParameters() is:"
                             << jsonObj;
    Q_UNUSED(ret);
    editor->show();

    if (QDialog::Accepted == editor->exec()) {
        QJsonObject params;
        QMetaObject::invokeMethod(editor,
                                  "parameters",
                                  Qt::DirectConnection,
                                  Q_RETURN_ARG(QJsonObject, params));
        qCInfo(mLoggingCategory) << "the parameter of parameters() is:"
                                 << params;
        QJsonDocument jsonDoc;
        jsonDoc.setObject(params);
        QString str = QString::fromUtf8(jsonDoc.toJson());
        mTableModelTool->addItem(str, index.row());

        afterRowEdited(index.row());
    }
}

bool SAKTableModelToolUi::append()
{
    QJsonObject jsonObj = mTableModelTool->itemContext(-1).toJsonObject();
    QDialog *editor = itemEditor();
    QGenericReturnArgument ret;
    QMetaObject::invokeMethod(editor,
                              "setParameters",
                              Qt::DirectConnection,
                              ret,
                              Q_ARG(QJsonObject, jsonObj));
    qCInfo(mLoggingCategory) << "the parameter of setParameters() is:"
                             << jsonObj;
    Q_UNUSED(ret);

    editor->show();
    if (!(QDialog::Accepted == editor->exec())) {
        return false;
    }

    QMetaObject::invokeMethod(editor,
                              "parameters",
                              Qt::DirectConnection,
                              Q_RETURN_ARG(QJsonObject, jsonObj));
    qCInfo(mLoggingCategory) << "the parameter of parameters() is:"
                             << jsonObj;
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QString str = QString::fromUtf8(jsonDoc.toJson());
    mTableModelTool->addItem(str, -1);
    return true;
}

QModelIndex SAKTableModelToolUi::currentIndex()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this,
                             tr("Please Select an Item"),
                             tr("Please select an tiem first,"
                                " then try again!"));
    }
    return index;
}

void SAKTableModelToolUi::writeToSettingsFile()
{
    QByteArray json = exportAsJson();
    SAKSettings::instance()->setValue(mItemsKey,
                                      QString::fromLatin1(json.toHex()));
}

bool SAKTableModelToolUi::isInitialized()
{
    if (!mTableModelTool) {
        QMessageBox::warning(this,
                             tr("Invalid Parameter"),
                             tr("The value of mTableModelTool is nullptr,"
                                " you must called initialize() first!"));
        return false;
    }

    return true;
}

void SAKTableModelToolUi::onPushButtonEditClicked()
{
    if (!isInitialized()) {
        return;
    }

    QModelIndex index = currentIndex();
    if (index.isValid()) {
        edit(index);
        writeToSettingsFile();
    }
}

void SAKTableModelToolUi::onPushButtonClearClicked()
{
    if (!isInitialized()) {
        return;
    }

    int ret = QMessageBox::warning(this,
                                   tr("Clear Data"),
                                   tr("The data will be empty from settings file, "
                                      "please confrim the operation!"),
                                   QMessageBox::Cancel|QMessageBox::Ok);
    if (ret == QMessageBox::Ok) {
        clear();
        writeToSettingsFile();
    }
}

void SAKTableModelToolUi::onPushButtonDeleteClicked()
{
    if (!isInitialized()) {
        return;
    }

    int ret = QMessageBox::warning(this,
                                   tr("Delete Data"),
                                   tr("The data will be delete from settings file, "
                                      "please confrim the operation!"),
                                   QMessageBox::Cancel|QMessageBox::Ok);

    if (ret != QMessageBox::Ok) {
        return;
    }

    QModelIndex index = currentIndex();
    if (index.isValid()) {
        remove(index);
        writeToSettingsFile();
    }
}

void SAKTableModelToolUi::onPushButtonImportClicked()
{
    if (!isInitialized()) {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Import data"),
                                                    ".",
                                                    tr("JSON (*.json);;All (*)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray json = file.readAll();
        file.close();;

        importFromJson(json);
        writeToSettingsFile();
    } else {
        qCWarning(mLoggingCategory) <<  "Can not open file:"
                                    << file.errorString();
    }
}

void SAKTableModelToolUi::onPushButtonExportClicked()
{
    if (!isInitialized()) {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Import data"),
                                                    ".",
                                                    tr("JSON (*.json);;All (*); "));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QFile::WriteOnly)) {
        QTextStream out(&file);
        out << exportAsJson();
        file.close();
    } else {
        qCWarning(mLoggingCategory) <<  "Can not open file:"
                                    << file.errorString();
    }
}

void SAKTableModelToolUi::onPushButtonAppendClicked()
{
    if (!isInitialized()) {
        return;
    }

    if (append()) {
        writeToSettingsFile();
    }
}
