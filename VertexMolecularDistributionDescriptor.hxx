#ifndef VERTEXMOLECULARDISTRIBUTIONDESCRIPTOR_HXX
#define VERTEXMOLECULARDISTRIBUTIONDESCRIPTOR_HXX
#include "VertexMolecularDistributionDescriptor.h"
#include <algorithm>

#include "itkLineIterator.h"
#include <itkImageFileWriter.h>


template<class TAJGraph, class TMolecularImage> VertexMolecularDistributionDescriptor<TAJGraph,TMolecularImage>::VertexMolecularDistributionDescriptor(){


}

template<class TAJGraph, class TMolecularImage> VertexMolecularDistributionDescriptor<TAJGraph,TMolecularImage>::~VertexMolecularDistributionDescriptor(){


}
template<class TAJGraph, class TMolecularImage> bool VertexMolecularDistributionDescriptor<TAJGraph,TMolecularImage>::PointInRegion(const PointType & point){
	return (m_CurrentOrigin-point).GetNorm()<=m_Radius;
}
template<class TAJGraph, class TMolecularImage> void VertexMolecularDistributionDescriptor<TAJGraph,TMolecularImage>::Compute(){

    //Foreach vertex
	this->Init();

    for(auto it=this->m_AJGraph->VerticesBegin();it!=this->m_AJGraph->VerticesEnd();it++){

    	this->Reset();
        //1. Initialize levelset to compute bisector planes

        AJVertexHandler vertexHandler=*it;

        if(this->m_AJGraph->AJVertexDegree(vertexHandler)!=3){
            continue;
        }


        PointType origin = this->m_AJGraph->GetAJVertex(vertexHandler)->GetPosition();

        std::vector<PointType> references;
        IndexType originIndex;

        this->m_MolecularImage->TransformPhysicalPointToIndex(origin,originIndex);

        auto neighbor = this->m_AJGraph->AdjacentAJVerticesBegin(vertexHandler);

        PointType pointA= this->m_AJGraph->GetAJVertex(*neighbor)->GetPosition();
        ++neighbor;

        PointType pointB= this->m_AJGraph->GetAJVertex(*neighbor)->GetPosition();
        ++neighbor;

        PointType pointC= this->m_AJGraph->GetAJVertex(*neighbor)->GetPosition();
        ++neighbor;

        assert(neighbor==this->m_AJGraph->AdjacentAJVerticesEnd(vertexHandler));

        auto vectorA= pointA-origin;
        auto vectorB= pointB-origin;
        auto vectorC= pointC-origin;

        vectorA.Normalize();
        vectorB.Normalize();
        vectorC.Normalize();

        auto bisectorAB=vectorA+vectorB;
        bisectorAB.Normalize();

        auto bisectorBC=vectorB + vectorC;
        bisectorBC.Normalize();

        auto bisectorCA=vectorC+vectorA;
        bisectorCA.Normalize();

        PointType originAB=origin + (m_Radius/2)*bisectorAB;
        PointType originBC=origin + (m_Radius/2)*bisectorBC;
        PointType originCA=origin + (m_Radius/2)*bisectorCA;

        IndexType indexAB, indexBC, indexCA;

        this->m_MolecularImage->TransformPhysicalPointToIndex(originAB,indexAB);
        this->m_MolecularImage->TransformPhysicalPointToIndex(originBC,indexBC);
        this->m_MolecularImage->TransformPhysicalPointToIndex(originCA,indexCA);
        this->SetNumberOfClusters(3);
        this->SetSeed(indexAB,0);
        this->SetSeed(indexBC,1);
        this->SetSeed(indexCA,2);

        m_CurrentOrigin=origin;
        m_ComputedFeatures[vertexHandler]=this->ComputeDescriptor();

    }
}

#endif
