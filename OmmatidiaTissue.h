/*
 * OmmatidiaTissue.h
 *
 *  Created on: Jun 12, 2015
 *      Author: morgan
 */

#ifndef OMMATIDIATISSUE_H_
#define OMMATIDIATISSUE_H_

#include "AJGraph.h"
#include "CellGraph.h"
#include "Cell.h"
#include "CellEdge.h"
#include "AJVertex.h"
#include "AJEdge.h"
#include <list>
#include <boost/bimap.hpp>
template<class TAJGraph,class TCellGraph> class Tissue : public itk::DataObject{
public:
	typedef Tissue Self;
	typedef itk::SmartPointer<Self> Pointer;
	typedef TAJGraph AJGraphType;
	typedef typename AJGraphType::AJVertexHandler AJVertexHandler;
	typedef typename AJGraphType::AJEdgeHandler AJEdgeHandler;

	typedef TCellGraph CellGraphType;
	typedef typename CellGraphType::CellType CellType;
	typedef typename CellGraphType::CellEdgeType CellEdgeType;

	typedef std::list<AJVertexHandler> TissuePerimeterVerticesContainer;
	typedef typename TissuePerimeterVerticesContainer::iterator TissuePerimeterVerticesIterator;

	typedef std::list<AJEdgeHandler> TissuePerimeterEdgesContainer;
	typedef typename TissuePerimeterEdgesContainer::iterator TissuePerimeterEdgesIterator;

	itkGetObjectMacro(AJGraph,AJGraphType);
	itkSetObjectMacro(AJGraph,AJGraphType);

	itkGetObjectMacro(CellGraph,CellGraphType);
	itkSetObjectMacro(CellGraph,CellGraphType);


	void AddAJVertexToTissuePerimeter(const AJVertexHandler & vertex){
		m_TissuePerimeterVertices.push_back(vertex);
	}
	void AddAJEdgeToTissuePerimeter(const AJEdgeHandler & edge){
		m_TissuePerimeterEdges.push_back(edge);
	}
	TissuePerimeterVerticesIterator BeginTissuePerimeterAJVertices(){
		return m_TissuePerimeterVertices.begin();
	}
	TissuePerimeterVerticesIterator EndTissuePerimeterAJVertices(){
		return m_TissuePerimeterVertices.end();
	}
	TissuePerimeterEdgesIterator BeginTissuePerimeterAJEdges(){
		return m_TissuePerimeterEdges.begin();
	}
	TissuePerimeterEdgesIterator EndTissuePerimeterAJEdges(){
		return m_TissuePerimeterEdges.end();
	}

	void AddAJEdgeToCellEdgeMapping(const typename AJGraphType::AJEdgeHandler & ajEdge,const typename CellGraphType::CellEdgeHandler & cellEdge){
		m_AJEdgeToCellEdgeMap.insert(ajEdge,cellEdge);

	}
	typename AJGraphType::AJEdgeHandler GetAJEdgeFromCellEdge(const typename CellGraphType::CellEdgeHandler & cellEdgeHandler){
		return m_AJEdgeToCellEdgeMap.left[cellEdgeHandler];

	}

	typename CellGraphType::CellEdgeHandler GetCellEdgeFromAJEdge(const typename AJGraphType::AJEdgeHandler & cellEdgeHandler){
		return m_AJEdgeToCellEdgeMap.right[cellEdgeHandler];
	}
	itkNewMacro(Self);
private:
	typename AJGraphType::Pointer m_AJGraph;
	typename CellGraphType::Pointer m_CellGraph;

	boost::bimap<typename AJGraphType::AJEdgeHandler,typename CellGraphType::CellEdgeHandler> m_AJEdgeToCellEdgeMap;

	TissuePerimeterVerticesContainer m_TissuePerimeterVertices;
	TissuePerimeterEdgesContainer    m_TissuePerimeterEdges;
};


template<int dim> using OmmatidiaAJGraph  = AJGraph<AJVertex<dim,unsigned long>,AJEdge<unsigned long> > ;
template<int dim> using OmmatidiaCellGraph = CellGraph<Cell<dim,unsigned long,typename OmmatidiaAJGraph<dim>::AJEdgeHandler>,CellEdge<typename OmmatidiaAJGraph<dim>::AJEdgeHandler > >;

template<int dim> using OmmatidiaTissue = Tissue<OmmatidiaAJGraph<dim>, OmmatidiaCellGraph<dim> >;


#endif /* OMMATIDIATISSUE_H_ */
