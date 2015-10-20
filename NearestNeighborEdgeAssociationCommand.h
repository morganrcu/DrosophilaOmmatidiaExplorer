/*
 * NearestNeighborCellAssociationCommand.h
 *
 *  Created on: Oct 20, 2015
 *      Author: morgan
 */

#ifndef NEARESTNEIGHBOREDGEASSOCIATIONCOMMAND_H_
#define NEARESTNEIGHBOREDGEASSOCIATIONCOMMAND_H_



#include <AJCorrespondenceSet.h>
#include "CellCorrespondence.h"



template<class TTissue> class NearestNeighborEdgeAssociationCommand {
public:
	typedef typename TTissue::AJGraphType::AJEdgeHandler EdgeHandler;
private:
	typename TTissue::Pointer m_Tissue0;
	typename TTissue::Pointer m_Tissue1;
	EdgeHandler m_Edge0;
	EdgeHandler m_Edge1;
public:

	NearestNeighborEdgeAssociationCommand(){

	}
	virtual ~NearestNeighborEdgeAssociationCommand(){

	}

	inline void SetTissue0(const typename TTissue::Pointer & tissue){
		m_Tissue0=tissue;
	}
	inline void SetTissue1(const typename TTissue::Pointer & tissue){
		m_Tissue1=tissue;
	}

	inline void SetEdge0(const EdgeHandler & edge){
		m_Edge0=edge;
	}
	inline EdgeHandler GetEdge1(){
		return m_Edge1;
	}

	virtual void Do();

};

#include "NearestNeighborEdgeAssociationCommand.hxx"
#endif /* MINCOSTMAXFLOWAJASSOCIATIONCOMMAND_H_ */




