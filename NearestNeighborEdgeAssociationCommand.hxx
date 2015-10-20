/*
 * NearestNeighborCellAssociationCommand.hxx
 *
 *  Created on: Oct 20, 2015
 *      Author: morgan
 */

#ifndef NEARESTNEIGHBOREDGEASSOCIATIONCOMMAND_HXX_
#define NEARESTNEIGHBOREDGEASSOCIATIONCOMMAND_HXX_

#include "NearestNeighborEdgeAssociationCommand.h"

#include "distancePointToLine.h"

template<class TEdge,class TTissue> double haussdorfDistanceEdgeToEdge(const TEdge & edgeA, const typename TTissue::Pointer & tissueEdgeA,const TEdge & edgeB,const typename TTissue::Pointer & tissueEdgeB){
	auto sourceA =tissueEdgeA->GetAJGraph()->GetAJEdgeSource(edgeA);
	auto positionSourceA =tissueEdgeA->GetAJGraph()->GetAJVertex(sourceA)->GetPosition();

	auto targetA =tissueEdgeA->GetAJGraph()->GetAJEdgeTarget(edgeA);
	auto positionTargetA =tissueEdgeA->GetAJGraph()->GetAJVertex(targetA)->GetPosition();

	auto sourceB =tissueEdgeB->GetAJGraph()->GetAJEdgeSource(edgeB);
	auto positionSourceB =tissueEdgeB->GetAJGraph()->GetAJVertex(sourceB)->GetPosition();

	auto targetB =tissueEdgeB->GetAJGraph()->GetAJEdgeTarget(edgeB);
	auto positionTargetB =tissueEdgeB->GetAJGraph()->GetAJVertex(targetB)->GetPosition();


	return std::max(std::max(distancePointToLine(positionSourceA,positionSourceB,positionTargetB),
							 distancePointToLine(positionTargetA,positionSourceB,positionTargetB)),
					std::max(distancePointToLine(positionSourceB,positionSourceA,positionTargetA),
							 distancePointToLine(positionTargetB,positionSourceA,positionTargetA))
					);
}


template<class TTissue> void NearestNeighborEdgeAssociationCommand<TTissue>::Do(){
		EdgeHandler nearestEdge;
	double minDistance = std::numeric_limits<double>::max();

	for(auto edgeIt = m_Tissue1->GetAJGraph()->EdgesBegin();edgeIt!=m_Tissue1->GetAJGraph()->EdgesEnd();++edgeIt){
		auto edgeHandler=*edgeIt;
		double distance=haussdorfDistanceEdgeToEdge<OmmatidiaTissue<3>::AJGraphType::AJEdgeHandler,OmmatidiaTissue<3> >(m_Edge0,m_Tissue0,edgeHandler,m_Tissue1);
		if(distance<minDistance){
			minDistance=distance;
			nearestEdge=edgeHandler;
		}
	}
	m_Edge1=nearestEdge;
}
#endif /* NEARESTNEIGHBORCELLASSOCIATIONCOMMAND_HXX_ */
