/*
 * OmmatidiaTissueStandardTreeItem.h
 *
 *  Created on: Oct 7, 2015
 *      Author: morgan
 */

#ifndef OMMATIDIATISSUESTANDARDTREEITEM_H_
#define OMMATIDIATISSUESTANDARDTREEITEM_H_
#include "OmmatidiaTissue.h"

class OmmatidiaTissueStandardTreeItem{
public:

	enum NodeType {ROOT,EDGE,EDGELIST,CELL,CELLLIST,VERTEX,VERTEXLIST,PROPERTY};

	explicit OmmatidiaTissueStandardTreeItem(const NodeType & type,const OmmatidiaTissue<3>::Pointer & tissue, OmmatidiaTissueStandardTreeItem * parentItem=NULL){
		m_pParent=parentItem;
		m_Tissue=tissue;
		m_Type=type;
	}

	virtual ~OmmatidiaTissueStandardTreeItem(){

	}

	virtual OmmatidiaTissueStandardTreeItem * child(int row){
		switch(m_Type){
		case ROOT:
			switch(row){
	    	case 0:
	    	{
	    		OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(CELLLIST,this->m_Tissue,this);

	    		for(auto it = m_Tissue->GetCellGraph()->CellsBegin();it!=m_Tissue->GetCellGraph()->CellsEnd();++it){
	    			auto cell = *it;
	    			result->AddCell(cell);
	    		}

	    		return result;
	    	}
	    	case 1:
	    	{
	    		OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(VERTEXLIST,this->m_Tissue,this);

	    		for(auto it = m_Tissue->GetAJGraph()->VerticesBegin();it!=m_Tissue->GetAJGraph()->VerticesEnd();++it){
	    			auto vertex = *it;
	    			result->AddVertex(vertex);
	    		}

	    		return result;

	    	}
			case 2:
			{
				OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(EDGELIST,this->m_Tissue,this);

				for(auto it = m_Tissue->GetAJGraph()->EdgesBegin();it!=m_Tissue->GetAJGraph()->EdgesEnd();++it){
					auto edge = *it;
					result->AddEdge(edge);
				}

				return result;
			}
			}
		case EDGE:
		{
			auto edge = m_Tissue->GetAJGraph()->GetAJEdge(m_AJEdge);
			auto cellPairs = edge->GetCellPair();
			OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(CELLLIST,m_Tissue,this);
			result->AddCell(cellPairs.first);
			result->AddCell(cellPairs.second);
			return result;
		}
		case EDGELIST:
		{
			OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(EDGE,m_Tissue,this);
			result->SetAJEdge(m_Edges[row]);
			return result;
		}
		case CELL:
		{
			auto cell = m_Tissue->GetCellGraph()->GetCell(m_Cell);
			OmmatidiaTissueStandardTreeItem * result;
			switch(row){
				case 0:
					result = new  OmmatidiaTissueStandardTreeItem(PROPERTY,m_Tissue,this);
					result->SetName(QString("x"));
					result->SetValue(QVariant(cell->GetPosition()[0]));
					return result;
				case 1:
					result = new  OmmatidiaTissueStandardTreeItem(PROPERTY,m_Tissue,this);

					result->SetName(QString("y"));
					result->SetValue(QVariant(cell->GetPosition()[1]));
					return result;
				case 2:
					result = new  OmmatidiaTissueStandardTreeItem(PROPERTY,m_Tissue,this);

					result->SetName(QString("z"));
					result->SetValue(QVariant(cell->GetPosition()[2]));
					return result;
				case 3:
				{
					OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(VERTEXLIST,this->m_Tissue,this);

					for(auto it = cell->BeginPerimeterAJVertices();it!=cell->EndPerimeterAJVertices();++it){
						auto vertex = *it;
						result->AddVertex(vertex);
					}
					return result;
				}
				case 4:
				{
					OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(EDGELIST,this->m_Tissue,this);

					for(auto it = cell->BeginPerimeterAJEdges();it!=cell->EndPerimeterAJEdges();++it){
						auto edge = *it;
						result->AddEdge(edge);
					}

					return result;
				}
			}
		}
		case CELLLIST:
		{
			OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(CELL,m_Tissue,this);
			result->SetCell(m_Cells[row]);
			return result;
		}
		case VERTEX:
		{
			auto vertex = m_Tissue->GetAJGraph()->GetAJVertex(m_AJVertex);
			OmmatidiaTissueStandardTreeItem * result;
			switch(row){
			case 0:
				result = new  OmmatidiaTissueStandardTreeItem(PROPERTY,m_Tissue,this);
				result->SetName(QString("x"));
				result->SetValue(QVariant(vertex->GetPosition()[0]));
				return result;
			case 1:
				result = new  OmmatidiaTissueStandardTreeItem(PROPERTY,m_Tissue,this);

				result->SetName(QString("y"));
				result->SetValue(QVariant(vertex->GetPosition()[1]));
				return result;
			case 2:
				result = new  OmmatidiaTissueStandardTreeItem(PROPERTY,m_Tissue,this);

				result->SetName(QString("z"));
				result->SetValue(QVariant(vertex->GetPosition()[2]));
				return result;
			case 3:
			{
				OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(CELLLIST,this->m_Tissue,this);
				for(auto it = vertex->BeginCells();it!=vertex->EndCells();++it){
					auto cell = *it;
					result->AddCell(cell);
				}
				return result;
			}
			case 4:
			{
				OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(EDGELIST,this->m_Tissue,this);

				for(auto it = m_Tissue->GetAJGraph()->BeginOutEdges(m_AJVertex);it!=m_Tissue->GetAJGraph()->EndOutEdges(m_AJVertex);++it){
					auto edge = *it;
					result->AddEdge(edge);
				}

				return result;
			}
			}
		}
		case VERTEXLIST:
		{
			OmmatidiaTissueStandardTreeItem * result = new OmmatidiaTissueStandardTreeItem(VERTEX,m_Tissue,this);
			result->SetAJVertex(m_Vertices[row]);
			return result;
		}

		case PROPERTY:
			return 0;
		};
		return 0;
	}
	virtual int childCount() const {
		switch(m_Type){
		case ROOT:
			return 3;
		case EDGE:
			return 1;
		case EDGELIST:
			return m_Edges.size();
		case CELL:
			return 5;
		case CELLLIST:
			return m_Cells.size();
		case VERTEX:
			return 5;
		case VERTEXLIST:
			return m_Vertices.size();
		case PROPERTY:
			return 0;
		};
		return 0;
	}
	virtual int columnCount() const {
		switch(m_Type){
			case ROOT:
				return 2;
			case EDGE:
				return 2;
			case EDGELIST:
				return 2;
			case CELL:
				return 2;
			case CELLLIST:
				return 2;
			case VERTEX:
				return 2;
			case VERTEXLIST:
				return 2;
			case PROPERTY:
				return 2;
		};
		return 2;
	}
	virtual QVariant data(int column) const {
		switch(m_Type){
			case ROOT:
				return QString("Tissue");
			case EDGE:
				return QString("Edge (%1,%2)").arg(m_Tissue->GetAJGraph()->GetAJEdgeSource(m_AJEdge)).arg(m_Tissue->GetAJGraph()->GetAJEdgeTarget(m_AJEdge));
			case EDGELIST:
				return QString("Edges");
			case CELL:
				return QString("Cell %1").arg(m_Cell);
			case CELLLIST:
				return QString("Cells");
			case VERTEX:
				return QString("Vertex %1").arg(m_AJVertex);
			case VERTEXLIST:
				return QString("Vertices");
			case PROPERTY:
				switch(column){
				case 0:
					return m_Name;
				case 1:
					return m_Value;
				};

		};
	}
	virtual int row() const{
		return 0;
	}

	virtual void AddVertex(typename OmmatidiaTissue<3>::AJGraphType::AJVertexHandler & vertex){
		m_Vertices.push_back(vertex);
	}
	virtual void AddEdge(typename OmmatidiaTissue<3>::AJGraphType::AJEdgeHandler & edge){
		m_Edges.push_back(edge);
	}
	virtual void AddCell(typename OmmatidiaTissue<3>::CellGraphType::CellVertexHandler & cell){
		m_Cells.push_back(cell);
	}
	virtual void SetAJVertex(typename OmmatidiaTissue<3>::AJGraphType::AJVertexHandler & vertex){
		m_AJVertex=vertex;
	}
	virtual void SetAJEdge(typename OmmatidiaTissue<3>::AJGraphType::AJEdgeHandler & edge){
		m_AJEdge=edge;
	}
	virtual void SetCell(typename OmmatidiaTissue<3>::CellGraphType::CellVertexHandler & cell){
		m_Cell=cell;
	}
	virtual void SetName(const QString & name){
		m_Name=name;
	}
	virtual void SetValue(const QVariant & value){
		m_Value=value;
	}
	std::vector<typename OmmatidiaTissue<3>::AJGraphType::AJVertexHandler> m_Vertices;
	std::vector<typename OmmatidiaTissue<3>::AJGraphType::AJEdgeHandler> m_Edges;
	std::vector<typename OmmatidiaTissue<3>::CellGraphType::CellVertexHandler> m_Cells;

	virtual  OmmatidiaTissueStandardTreeItem* parentItem(){
		return m_pParent;
	}
	OmmatidiaTissueStandardTreeItem * m_pParent;
	OmmatidiaTissue<3>::Pointer m_Tissue;
	NodeType m_Type;
	typename OmmatidiaTissue<3>::AJGraphType::AJVertexHandler m_AJVertex;
	typename OmmatidiaTissue<3>::AJGraphType::AJEdgeHandler m_AJEdge;
	typename OmmatidiaTissue<3>::CellGraphType::CellVertexHandler m_Cell;

	QString m_Name;
	QVariant m_Value;
};


#endif /* OMMATIDIATISSUESTANDARDTREEITEM_H_ */
