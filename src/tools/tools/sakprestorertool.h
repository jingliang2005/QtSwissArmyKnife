/*******************************************************************************
 * Copyright 2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#ifndef SAKPRESTORERTOOL_H
#define SAKPRESTORERTOOL_H

#include <QTimer>
#include <QMutex>
#include <QVariant>
#include <QAbstractTableModel>

#include "saktablemodeltool.h"

class SAKPrestorerTool : public SAKTableModelTool
{
    Q_OBJECT
    Q_PROPERTY(QVariant tableModel READ tableModel CONSTANT)
    Q_PROPERTY(QStringList headers READ headers CONSTANT)

    Q_PROPERTY(QString itemDescription READ itemDescription CONSTANT)
    Q_PROPERTY(QString itemTextFormat READ itemTextFormat CONSTANT)
    Q_PROPERTY(QString itemEscapeCharacter READ itemEscapeCharacter CONSTANT)
    Q_PROPERTY(QString itemPrefix READ itemPrefix CONSTANT)
    Q_PROPERTY(QString itemSuffix READ itemSuffix CONSTANT)
    Q_PROPERTY(QString itemCrcEnable READ itemCrcEnable CONSTANT)
    Q_PROPERTY(QString itemCrcBigEndian READ itemCrcBigEndian CONSTANT)
    Q_PROPERTY(QString itemCrcAlgorithm READ itemCrcAlgorithm CONSTANT)
    Q_PROPERTY(QString itemCrcStartIndex READ itemCrcStartIndex CONSTANT)
    Q_PROPERTY(QString itemCrcEndIndex READ itemCrcEndIndex CONSTANT)
    Q_PROPERTY(QString itemText READ itemText CONSTANT)

    Q_PROPERTY(QStringList descriptions READ descriptions NOTIFY descriptionsChanged)
public:
    struct Item {
        QString itemDescription{"Demo"};
        int itemTextFormat;
        int itemEscapeCharacter;
        int itemPrefix;
        QString itemText;
        int itemSuffix;

        bool itemCrcEnable;
        bool itemCrxBigEndian;
        int itemCrcAlgorithm;
        int itemCrcStartIndex;
        int itemCrcEndIndex;
    };

    struct ItemKeys {
        const QString itemDescription{"Description"};
        const QString itemTextFormat{"Format"};
        const QString itemEscapeCharacter{"Escape"};
        const QString itemPrefix{"Prefix"};
        const QString itemSuffix{"Suffix"};
        const QString itemCrcEnable{"CrcEnable"};
        const QString itemCrcBigEndian{"BigEndian"};
        const QString itemCrcAlgorithm{"Algorithm"};
        const QString itemCrcStartIndex{"Start"};
        const QString itemCrcEndIndex{"End"};
        const QString itemText{"Data"};
    };

public:
    explicit SAKPrestorerTool(QObject *parent = nullptr);
    virtual QString cookHeaderString(const QString &str) override;
    Q_INVOKABLE virtual QVariant itemContext(int index) final;
    QString description(int index);

    Q_INVOKABLE void send(int index);

protected:
    virtual void inputBytes(const QByteArray &bytes,
                            const QVariant &context) final;
    void run() final;

    virtual int rowCount(const QModelIndex &parent
                         = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent
                            = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value,
                         int role = Qt::EditRole) override;
    virtual bool insertRows(int row, int count,
                            const QModelIndex &parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count,
                            const QModelIndex &parent = QModelIndex()) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;

private:
    void try2send();

private:
    const int mScanInterval{5};
    QList<int> mIndexs;
    QMutex mIndexsMutex;
private:
    QVector<Item> mItems;
    QMutex mItemsMutex;
    struct ItemKeys mDataKeys;
    const int mTableColumnCount{11};

private:
    QByteArray itemBytes(const Item &item);
    QVariant columnDisplayRoleData(const Item &item, int column) const;

private:
    QString itemDescription();
    QString itemTextFormat();
    QString itemEscapeCharacter();
    QString itemPrefix();
    QString itemSuffix();
    QString itemCrcEnable();
    QString itemCrcBigEndian();
    QString itemCrcAlgorithm();
    QString itemCrcStartIndex();
    QString itemCrcEndIndex();
    QString itemText();

    QStringList descriptions();

Q_SIGNALS:
    void descriptionsChanged();
};

#endif // EDPRESETDATATOOL_NEW_H
