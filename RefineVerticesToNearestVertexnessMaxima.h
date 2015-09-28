/*
 * RefineVerticesToNearestVertexnessMaxima.h
 *
 *  Created on: Mar 12, 2015
 *      Author: morgan
 */

#ifndef REFINEVERTICESTONEARESTVERTEXNESSMAXIMA_H_
#define REFINEVERTICESTONEARESTVERTEXNESSMAXIMA_H_

#include "itkObject.h"

template<class TVerticesContainer, class TVertexnessImage> class RefineVerticesToNearestVertexnessMaxima : public itk::Object{

public:

	typedef RefineVerticesToNearestVertexnessMaxima<TVerticesContainer,TVertexnessImage> Self;
	typedef itk::SmartPointer<Self> Pointer;

	itkNewMacro(Self);

	typedef TVerticesContainer VerticesContainerType;
	itkGetObjectMacro(Vertices,VerticesContainerType)
	itkSetObjectMacro(Vertices,VerticesContainerType)

	typedef TVertexnessImage VertexnessImageType;
	itkGetObjectMacro(VertexnessImage,VertexnessImageType)
	itkSetObjectMacro(VertexnessImage,VertexnessImageType)

	virtual void Compute(){

		bool end=false;
		while(!end){
			auto it = m_Vertices->VerticesBegin();
			bool stop=true;
			while(it!=m_Vertices->VerticesEnd()){
				if(m_Vertices->AJVertexDegree(*it)==0){
					m_Vertices->DeleteAJVertex(*it);
					stop=false;
					break;
				}
				++it;
			}
			if(stop){
				end=true;
			}
		}

		for(auto it = m_Vertices->VerticesBegin();it!=m_Vertices->VerticesEnd();it++ ){
			auto position = m_Vertices->GetAJVertex(*it)->GetPosition();
			typedef typename VertexnessImageType::IndexType IndexType;
			IndexType index;

			m_VertexnessImage->TransformPhysicalPointToIndex(position,index);

			bool isMaxima=false;
			std::cout << "Refining " << index << std::endl;

			while(!isMaxima){
				isMaxima=true;

				if(m_VertexnessImage->GetBufferedRegion().IsInside(index)){
					IndexType localMax;
					double maxValue=m_VertexnessImage->GetPixel(index);
					itk::Offset<3> offset;
					for(offset[0] =-1;offset[0]<2;offset[0]++){
						for(offset[1] =-1;offset[1]<2;offset[1]++){
							for(offset[2] =-1;offset[2]<2;offset[2]++){
								if(offset[0]==0 && offset[1]==0 && offset[2]==0) continue;

								IndexType neighIndex= index +offset;
								if(!m_VertexnessImage->GetBufferedRegion().IsInside(neighIndex)) continue;
								if(this->m_VertexnessImage->GetPixel(neighIndex)>maxValue){
									isMaxima=false;
									maxValue=this->m_VertexnessImage->GetPixel(neighIndex);
									localMax=neighIndex;
								}
							}
						}
					}
					if(!isMaxima){
						index=localMax;
						std::cout << "\t " << index << std::endl;
					}
				}else{
					isMaxima=true;
				}
			}
			m_VertexnessImage->TransformIndexToPhysicalPoint(index,position);
			m_Vertices->GetAJVertex(*it)->SetPosition(position);
		}
	}


protected:
	RefineVerticesToNearestVertexnessMaxima(){

	}
	~RefineVerticesToNearestVertexnessMaxima(){

	}

private:

	typename VerticesContainerType::Pointer m_Vertices;
	typename VertexnessImageType::Pointer m_VertexnessImage;
};


#endif /* REFINEVERTICESTONEARESTVERTEXNESSMAXIMA_H_ */
