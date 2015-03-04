/*
 * SeedBasedMolecularDistributionDescriptor.hxx
 *
 *  Created on: Feb 26, 2015
 *      Author: morgan
 */

#ifndef SEEDBASEDMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_
#define SEEDBASEDMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_

#include "SeedBasedMolecularDistributionDescriptor.h"
#include "FeatureDescriptor.h"
template<class TMolecularImage> SeedBasedMolecularDistributionDescriptor<TMolecularImage>::SeedBasedMolecularDistributionDescriptor(){

}

template<class TMolecularImage> SeedBasedMolecularDistributionDescriptor<TMolecularImage>::~SeedBasedMolecularDistributionDescriptor(){

}

template<class TMolecularImage> void SeedBasedMolecularDistributionDescriptor<TMolecularImage>::Init(){
    this->m_LabelImage=LabelImageType::New();
    this->m_LabelImage->SetRegions(this->m_MolecularImage->GetLargestPossibleRegion());
    this->m_LabelImage->CopyInformation(this->m_MolecularImage);
    this->m_LabelImage->Allocate();

    this->m_LevelSetImage=LevelSetImageType::New();
    this->m_LevelSetImage->CopyInformation(this->m_MolecularImage);
    this->m_LevelSetImage->SetRegions(this->m_MolecularImage->GetLargestPossibleRegion());
    this->m_LevelSetImage->Allocate();


    this->m_ClusterImage=ClusterImageType::New();
    this->m_ClusterImage->SetRegions(this->m_MolecularImage->GetLargestPossibleRegion());
    this->m_ClusterImage->CopyInformation(this->m_MolecularImage);
    this->m_ClusterImage->Allocate();
}
template<class TMolecularImage> void SeedBasedMolecularDistributionDescriptor<TMolecularImage>::SetSeed(IndexType & index, const ClusterDescriptor & cluster){

		typename TMolecularImage::RegionType region=this->m_LabelImage->GetLargestPossibleRegion();

		if(!region.IsInside(index)){
			for(int i=0;i<3;i++){
				if(index[i]<region.GetIndex(i)){
					index[i]=region.GetIndex(i);
				}
				if(region.GetIndex(i)+region.GetSize(i)<index[i]){
					index[i]=region.GetIndex(i)+region.GetSize(i)-1;
				}
			}

		}

	   this->m_LabelImage->SetPixel(index,InitialTrialPoint);
	   this->m_LevelSetImage->SetPixel(index,0);
	   this->m_ClusterImage->SetPixel(index,cluster);

       LevelSetNodeType node;


       node.SetIndex(index);
       node.SetValue(0);

       this->m_TrialHeap.push(node);

}

template<class TMolecularImage> void SeedBasedMolecularDistributionDescriptor<TMolecularImage>::SetBanned(const IndexType & index){
	this->m_LabelImage->SetPixel(index,OutsidePoint);
}

template<class TMolecularImage> void SeedBasedMolecularDistributionDescriptor<TMolecularImage>::Reset(){
    //std::for_each(this->m_AJGraph->VerticesBegin(),_AJGraph->VerticesEnd(),);

    this->m_LabelImage->FillBuffer(FarPoint);
    this->m_LevelSetImage->FillBuffer(itk::NumericTraits<double>::max());
    this->m_ClusterImage->FillBuffer(itk::NumericTraits<unsigned short>::max());
}

template<class TMolecularImage> itk::Array<double> SeedBasedMolecularDistributionDescriptor<TMolecularImage>::ComputeDescriptor(){




    // process points on the heap
    LevelSetNodeType node;
    double       centerValue;
    double       oldProgress = 0;

    std::list<IndexType> visitedRegion;
    // CACHE
    while ( !m_TrialHeap.empty() )
      {
      // get the node with the smallest value
      node = m_TrialHeap.top();
      m_TrialHeap.pop();


      IndexType centerIndex = node.GetIndex();
      PointType centerPoint;
      this->m_MolecularImage->TransformIndexToPhysicalPoint(centerIndex,centerPoint);

      // does this node contain the current value ?
      centerValue = static_cast< double >( m_LevelSetImage->GetPixel( node.GetIndex() ) );


      if(centerValue!=node.GetValue()){
    	  //std::cout << "Skipping Unstacked " << centerIndex << " Value: " << centerValue << std::endl;
    	  continue;
      }else{
    	  //std::cout << "Unstacked " << centerIndex << " Value: " << centerValue << std::endl;
      }

      //std::cout << "\t TrialHeapSize: " << m_TrialHeap.size() << std::endl;


      if ( m_LabelImage->GetPixel( node.GetIndex() ) == AlivePoint ||  m_LabelImage->GetPixel( node.GetIndex() ) == InitialTrialPoint ){
          // set this node as alive
          m_LabelImage->SetPixel(node.GetIndex(), VisitedPoint);
          visitedRegion.push_back(node.GetIndex());
          unsigned char label;

          assert(m_ClusterImage->GetBufferedRegion().IsInside(centerIndex));
          unsigned short centerCluster = m_ClusterImage->GetPixel(centerIndex);

          itk::Offset<3> offset;
          for(offset[0] =-1;offset[0]<2;offset[0]++){
            for(offset[1] =-1;offset[1]<2;offset[1]++){
                for(offset[2] =-1;offset[2]<2;offset[2]++){

                    if(offset[0]==0 && offset[1]==0 && offset[2]==0) continue;

                    IndexType neighIndex= centerIndex +offset;
                    if(!m_MolecularImage->GetBufferedRegion().IsInside(neighIndex)) continue;

                    if(this->m_LabelImage->GetPixel(neighIndex)==OutsidePoint){
                    	continue;
                    }

                    if(this->m_LabelImage->GetPixel(neighIndex)==FarPoint || this->m_LabelImage->GetPixel(neighIndex)==AlivePoint ){
                    	//if(neighCluster!=itk::NumericTraits<unsigned short>::max() && neighCluster!=centerCluster) continue;

                        PointType neighPoint;

                        this->m_MolecularImage->TransformIndexToPhysicalPoint(neighIndex,neighPoint);
                        if(this->PointInRegion(neighPoint)){

                            double newValue =centerValue + (centerPoint-neighPoint).GetNorm();
                            double oldValue =this->m_LevelSetImage->GetPixel(neighIndex);

                            if(newValue<oldValue){
								this->m_LabelImage->SetPixel(neighIndex,AlivePoint);
								this->m_LevelSetImage->SetPixel(neighIndex,newValue);

								assert(m_ClusterImage->GetBufferedRegion().IsInside(neighIndex));
								this->m_ClusterImage->SetPixel(neighIndex,centerCluster);

								LevelSetNodeType node;
								node.SetIndex(neighIndex);
								node.SetValue(newValue);
								//std::cout << "\t\t Stacked " << neighIndex << " Value: " << newValue << std::endl;
								this->m_TrialHeap.push(node);
                            }else{
                            	//std::cout << "\t\t Skipping Stacked " << neighIndex << " Value: " << newValue  << ">" << oldValue << std::endl;
                            }
                        }
                    }

                }
            }
          }
      }
    }


    itk::Array<double> distribution(m_NumberOfClusters);
	itk::Array<int> volume(m_NumberOfClusters);

	for(int i=0;i<distribution.size();i++){
		distribution[i]=0;
	}

    volume.Fill(0);
	for(IndexType index : visitedRegion) {
    	unsigned short cluster=this->m_ClusterImage->GetPixel(index);
    	distribution[cluster]+= this->m_MolecularImage->GetPixel(index);
    	volume[cluster]++;
    }
    for(int i=0;i<m_NumberOfClusters;i++){
    	distribution[i]=distribution[i]/volume[i];
    }

return distribution;

}

#endif /* SEEDBASEDMOLECULARDISTRIBUTIONDESCRIPTOR_HXX_ */
