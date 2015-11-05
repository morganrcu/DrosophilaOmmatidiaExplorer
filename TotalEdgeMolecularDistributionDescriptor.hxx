#ifndef TOTALEDGEMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_
#define TOTALEDGEMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_

#include "TotalEdgeMolecularDistributionDescriptor.h"
#include <algorithm>

#include "itkLineIterator.h"
#include <itkImageFileWriter.h>


template<class TAJGraph, class TMolecularImage> TotalEdgeMolecularDistributionDescriptor<TAJGraph, TMolecularImage>::TotalEdgeMolecularDistributionDescriptor(){


}

template<class TAJGraph, class TMolecularImage> TotalEdgeMolecularDistributionDescriptor<TAJGraph, TMolecularImage>::~TotalEdgeMolecularDistributionDescriptor(){


}
template<class TAJGraph, class TMolecularImage> bool TotalEdgeMolecularDistributionDescriptor<TAJGraph, TMolecularImage>::PointInRegion(const PointType & p){

	auto wv = (m_Point1 - m_Point0);
	double l2 = wv.GetSquaredNorm();
	if (l2 == 0){
		return (p - m_Point0).GetNorm() <= m_Radius;
	}
	else{
		auto pv = (p - this->m_Point0);
		double t = (pv*wv) / l2;
		if (t<0.0){
			return (p - m_Point0).GetNorm() <= m_Radius;
		}
		else if (t>1.0){
			return (p - m_Point1).GetNorm() <= m_Radius;
		}
		else{
			auto projection = m_Point0 + t * wv;  // Projection falls on the segment
			return (p - projection).GetNorm() <= m_Radius;
		}
	}

}
template<class TAJGraph, class TMolecularImage> void TotalEdgeMolecularDistributionDescriptor<TAJGraph, TMolecularImage>::Compute(){

	//Foreach vertex
	this->Init();
	typedef itk::Vector<double, 3> VectorType;
	VectorType upDir;
	upDir.Fill(0);
	upDir[2] = 1;

	for (auto it = this->m_AJGraph->EdgesBegin(); it != this->m_AJGraph->EdgesEnd(); it++){

		this->Reset();

		AJEdgeHandler edgeHandler = *it;

		PointType source = this->m_AJGraph->GetAJVertex(this->m_AJGraph->GetAJEdgeSource(edgeHandler))->GetPosition();
		PointType target = this->m_AJGraph->GetAJVertex(this->m_AJGraph->GetAJEdgeTarget(edgeHandler))->GetPosition();

		PointType midPoint = source + 0.5*(target - source);
		std::vector<PointType> references;
		IndexType sourceIndex, targetIndex;
		IndexType midPointIndex;

		this->m_MolecularImage->TransformPhysicalPointToIndex(midPoint, midPointIndex);

		this->SetNumberOfClusters(1);
		
		this->SetSeed(midPointIndex, 0);

		m_Point0 = source;
		m_Point1 = target;
		m_AJGraph->GetAJEdge(edgeHandler)->SetDescriptor(this->ComputeDescriptor());


	}
}

#endif /* TOTALEDGEMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_ */
