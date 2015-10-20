/*
 * NearestNeighborCellAssociationCommand.hxx
 *
 *  Created on: Oct 20, 2015
 *      Author: morgan
 */

#ifndef NEARESTNEIGHBORAJGRAPHASSOCIATIONCOMMAND_HXX_
#define NEARESTNEIGHBORAJGRAPHASSOCIATIONCOMMAND_HXX_

#include "NearestNeighborCellAssociationCommand.h"
#include "distancePointToLine.h"
template<class TCell,class TTissue> static double haussdorfDistanceFaceToFace(const typename TCell::Pointer & cellA, const typename TTissue::Pointer & tissueA,const typename TCell::Pointer & cellB, const typename TTissue::Pointer & tissueB ){

	double max=0;

	for(auto vertexIt= cellA->BeginPerimeterAJVertices();vertexIt != cellA->EndPerimeterAJVertices();++vertexIt){

		double min=std::numeric_limits<double>::max();
		auto posX = tissueA->GetAJGraph()->GetAJVertex(*vertexIt)->GetPosition();
		for(auto edgeIt =cellB->BeginPerimeterAJEdges();edgeIt != cellB->EndPerimeterAJEdges();++edgeIt){

			auto source = tissueB->GetAJGraph()->GetAJEdgeSource(*edgeIt);
			auto sourcePoint = tissueB->GetAJGraph()->GetAJVertex(source)->GetPosition();
			auto target = tissueB->GetAJGraph()->GetAJEdgeTarget(*edgeIt);
			auto targetPoint = tissueB->GetAJGraph()->GetAJVertex(target)->GetPosition();

			min = std::min(min,distancePointToLine(posX,sourcePoint,targetPoint));
		}
		max = std::max(max,min);
	}

	for(auto vertexIt= cellB->BeginPerimeterAJVertices();vertexIt != cellB->EndPerimeterAJVertices();++vertexIt){

		double min=std::numeric_limits<double>::max();
		auto posX = tissueB->GetAJGraph()->GetAJVertex(*vertexIt)->GetPosition();
		for(auto edgeIt =cellA->BeginPerimeterAJEdges();edgeIt != cellA->EndPerimeterAJEdges();++edgeIt){

			auto source = tissueA->GetAJGraph()->GetAJEdgeSource(*edgeIt);
			auto sourcePoint = tissueA->GetAJGraph()->GetAJVertex(source)->GetPosition();
			auto target = tissueA->GetAJGraph()->GetAJEdgeTarget(*edgeIt);
			auto targetPoint = tissueA->GetAJGraph()->GetAJVertex(target)->GetPosition();

			min = std::min(min,distancePointToLine(posX,sourcePoint,targetPoint));
		}
		max = std::max(max,min);
	}
	return max;

}
template<class TTissue> void NearestNeighborCellAssociationCommand<TTissue>::Do(){
	auto cell0= this->m_Tissue0->GetCellGraph()->GetCell(m_Cell0);

	CellVertexHandler nearestCell;
	double minDistance = std::numeric_limits<double>::max();

	for(auto cellIt = m_Tissue1->GetCellGraph()->CellsBegin();cellIt!=m_Tissue1->GetCellGraph()->CellsEnd();++cellIt){
		auto cellHandler=*cellIt;
		auto cell1 = m_Tissue1->GetCellGraph()->GetCell(cellHandler);
		double distance=haussdorfDistanceFaceToFace<OmmatidiaTissue<3>::CellGraphType::CellType,OmmatidiaTissue<3> >(cell0,m_Tissue0,cell1,m_Tissue1);
		if(distance<minDistance){
			minDistance=distance;
			nearestCell=cellHandler;
		}
	}
	m_Cell1=nearestCell;
}
#endif /* NEARESTNEIGHBORCELLASSOCIATIONCOMMAND_HXX_ */
