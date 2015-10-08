/*
 * OmmatidiaTissueTreeModel.h
 *
 *  Created on: Sep 30, 2015
 *      Author: morgan
 */

#ifndef OMMATIDIATISSUETREEMODEL_H_
#define OMMATIDIATISSUETREEMODEL_H_

#include <qabstractitemmodel.h>
//#include "OmmatidiaTissueTreeRootItem.h"
#include "OmmatidiaTissueStandardTreeItem.h"
#include <OmmatidiaTissue.h>

class OmmatidiaTissueTreeModel: public QAbstractItemModel {
public:
	 Q_OBJECT

	public:
	    explicit OmmatidiaTissueTreeModel(const OmmatidiaTissue<3>::Pointer & tissue, QObject *parent = 0);
	    ~OmmatidiaTissueTreeModel();

	    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
	    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
	    QVariant headerData(int section, Qt::Orientation orientation,
	                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	    QModelIndex index(int row, int column,
	                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
	    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

	private:
	    OmmatidiaTissueStandardTreeItem *m_pRootItem;
};

#endif /* OMMATIDIATISSUETREEMODEL_H_ */
