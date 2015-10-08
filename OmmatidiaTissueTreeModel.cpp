/*
 * OmmatidiaTissueTreeModel.cpp
 *
 *  Created on: Sep 30, 2015
 *      Author: morgan
 */

#include <OmmatidiaTissueTreeModel.h>

OmmatidiaTissueTreeModel::OmmatidiaTissueTreeModel(const OmmatidiaTissue<3>::Pointer & tissue, QObject *parent)
: QAbstractItemModel(parent)
{
	m_pRootItem = new OmmatidiaTissueStandardTreeItem(OmmatidiaTissueStandardTreeItem::ROOT,tissue,NULL);
}

OmmatidiaTissueTreeModel::~OmmatidiaTissueTreeModel() {

    delete m_pRootItem;

}


QModelIndex OmmatidiaTissueTreeModel::index(int row, int column, const QModelIndex &parent)
            const
{

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    OmmatidiaTissueStandardTreeItem *parentItem;

    if (!parent.isValid())
        parentItem =m_pRootItem;
    else
        parentItem = static_cast<OmmatidiaTissueStandardTreeItem*>(parent.internalPointer());

    OmmatidiaTissueStandardTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

}
QModelIndex OmmatidiaTissueTreeModel::parent(const QModelIndex &index) const
{

    if (!index.isValid())
        return QModelIndex();

    OmmatidiaTissueStandardTreeItem *childItem = static_cast<OmmatidiaTissueStandardTreeItem*>(index.internalPointer());
    OmmatidiaTissueStandardTreeItem *parentItem = childItem->parentItem();

    if (parentItem == m_pRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);

}
int OmmatidiaTissueTreeModel::rowCount(const QModelIndex &parent) const
{

	OmmatidiaTissueStandardTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_pRootItem;
    else
        parentItem = static_cast<OmmatidiaTissueStandardTreeItem*>(parent.internalPointer());

    return parentItem->childCount();

}
int OmmatidiaTissueTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<OmmatidiaTissueStandardTreeItem*>(parent.internalPointer())->columnCount();
    else
        return m_pRootItem->columnCount();

}
QVariant OmmatidiaTissueTreeModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    OmmatidiaTissueStandardTreeItem *item = static_cast<OmmatidiaTissueStandardTreeItem*>(index.internalPointer());

    return item->data(index.column());

}
Qt::ItemFlags OmmatidiaTissueTreeModel::flags(const QModelIndex &index) const
{

    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);

}
QVariant OmmatidiaTissueTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_pRootItem->data(section);

    return QVariant();
}
