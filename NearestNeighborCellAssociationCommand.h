/*
 * NearestNeighborCellAssociationCommand.h
 *
 *  Created on: Oct 20, 2015
 *      Author: morgan
 */

#ifndef NEARESTNEIGHBORCELLASSOCIATIONCOMMAND_H_
#define NEARESTNEIGHBORCELLASSOCIATIONCOMMAND_H_



#include <AJCorrespondenceSet.h>
#include "CellCorrespondence.h"



template<class TTissue> class NearestNeighborCellAssociationCommand {
public:
	typedef typename TTissue::CellGraphType::CellVertexHandler CellVertexHandler;
private:
	typename TTissue::Pointer m_Tissue0;
	typename TTissue::Pointer m_Tissue1;
	CellVertexHandler m_Cell0;
	CellVertexHandler m_Cell1;
public:

	NearestNeighborCellAssociationCommand(){

	}
	virtual ~NearestNeighborCellAssociationCommand(){

	}

	inline void SetTissue0(const typename TTissue::Pointer & tissue){
		m_Tissue0=tissue;
	}
	inline void SetTissue1(const typename TTissue::Pointer & tissue){
		m_Tissue1=tissue;
	}

	inline void SetCell0(const CellVertexHandler & cell){
		m_Cell0=cell;
	}
	inline CellVertexHandler GetCell1(){
		return m_Cell1;
	}

	virtual void Do();

};

#include "NearestNeighborCellAssociationCommand.hxx"
#endif /* MINCOSTMAXFLOWAJASSOCIATIONCOMMAND_H_ */




