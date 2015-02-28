/*
 * EdgeMolecularDistributionDescriptor.hxx
 *
 *  Created on: Feb 26, 2015
 *      Author: morgan
 */

#ifndef EDGEMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_
#define EDGEMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_

#include "EdgeMolecularDistributionDescriptor.h"
#include <algorithm>

#include "itkLineIterator.h"
#include <itkImageFileWriter.h>


template<class TAJGraph, class TMolecularImage> EdgeMolecularDistributionDescriptor<TAJGraph,TMolecularImage>::EdgeMolecularDistributionDescriptor(){


}

template<class TAJGraph, class TMolecularImage> EdgeMolecularDistributionDescriptor<TAJGraph,TMolecularImage>::~EdgeMolecularDistributionDescriptor(){


}
template<class TAJGraph, class TMolecularImage> bool EdgeMolecularDistributionDescriptor<TAJGraph,TMolecularImage>::PointInRegion(const PointType & p){

	auto wv =(m_Point1-m_Point0);
	double l2=wv.GetSquaredNorm();
	if(l2==0){
		return (p - m_Point0 ).GetNorm()<=m_Radius;
	}else{
		auto pv = (p - this->m_Point0);
		double t=(pv*wv)/l2;
		if(t<0.0){
			return (p -m_Point0 ).GetNorm()<=m_Radius;
		}else if(t>1.0){
			return (p -m_Point1 ).GetNorm()<=m_Radius;
		}else{
			auto projection = m_Point0 + t * wv;  // Projection falls on the segment
			return (p-projection).GetNorm()<=m_Radius;
		}
	}

}
template<class TAJGraph, class TMolecularImage> void EdgeMolecularDistributionDescriptor<TAJGraph,TMolecularImage>::Compute(){

    //Foreach vertex
	this->Init();
	typedef itk::Vector<double,3> VectorType;
	VectorType upDir;
	upDir.Fill(0);
	upDir[2]=1;
    for(auto it=this->m_AJGraph->EdgesBegin();it!=this->m_AJGraph->EdgesEnd();it++){

    	this->Reset();

        AJEdgeHandler edgeHandler=*it;

        PointType source = this->m_AJGraph->GetAJVertex(this->m_AJGraph->GetAJEdgeSource(edgeHandler))->GetPosition();
        PointType target = this->m_AJGraph->GetAJVertex(this->m_AJGraph->GetAJEdgeTarget(edgeHandler))->GetPosition();

        std::vector<PointType> references;
        IndexType sourceIndex,targetIndex;

        this->m_MolecularImage->TransformPhysicalPointToIndex(source,sourceIndex);
        this->m_MolecularImage->TransformPhysicalPointToIndex(target,targetIndex);

        auto v = (target-source)/m_NumberOfSegments;
        auto vnormal=v/v.GetNorm();
        VectorType normal;

        normal[0]=vnormal[1]*upDir[2]-vnormal[2]*upDir[1];
        normal[1]=vnormal[2]*upDir[0]-vnormal[0]*upDir[2];
        normal[2]=vnormal[0]*upDir[1]-vnormal[1]*upDir[0];
        normal.Normalize();
        this->SetNumberOfClusters(m_NumberOfSegments*2);
        for(int s = 0;s<m_NumberOfSegments;s++){



        	PointType originA= source + (s+0.5)*v + normal*m_Radius/2;
        	IndexType indexA;

            this->m_MolecularImage->TransformPhysicalPointToIndex(originA,indexA);
            this->SetSeed(indexA,2*s);

            PointType originB= source + (s+0.5)*v - normal*m_Radius/2;
            IndexType indexB;

            this->m_MolecularImage->TransformPhysicalPointToIndex(originB,indexB);
            this->SetSeed(indexB,2*s+1);

        }




        this->m_Point0=source + m_Radius*vnormal;
        this->m_Point1=target - m_Radius*vnormal;

        m_ComputedFeatures[edgeHandler]=this->ComputeDescriptor();

    }
}

#endif /* EDGEMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_ */
