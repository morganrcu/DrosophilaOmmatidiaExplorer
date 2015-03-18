/*=========================================================================

  authors: Bryn A. Lloyd, Simon K. Warfield, Computational Radiology Laborotory (CRL), Brigham and Womans
  date: 06/30/2005
  
  Acknowledgements:
This investigation was supported in part by NSF ITR 0426558 and 
NIH grants R21 MH67054 and P41 RR13218.

=========================================================================*/
#ifndef _LocalMaximaImageFilter_hxx
#define _LocalMaximaImageFilter_hxx
#include "LocalMaximaImageFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"

#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkConstantBoundaryCondition.h"

#include "itkOffset.h"
#include "itkProgressReporter.h"

#include "itkNumericTraits.h"

#include <iostream>
template <class TInputImage,class TOutputContainer> LocalMaximaImageFilter< TInputImage,TOutputContainer>::LocalMaximaImageFilter()
{

  this->m_Radius.Fill(1);
  this->m_Threshold = 0.005;

  m_LocalMaxima = TOutputContainer::New();
}



template <class TInputImage,class TOutputContainer> void LocalMaximaImageFilter< TInputImage,TOutputContainer>::Compute() {


    InputImageConstPointer input = this->GetInputImage();


	unsigned int i;
    itk::ConstantBoundaryCondition<InputImageType> cbc;
    cbc.SetConstant(itk::NumericTraits<InputPixelType>::NonpositiveMin());

    itk::ConstNeighborhoodIterator<InputImageType> bit;

	// Find the data-set boundary "faces"
    typename itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
    itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> bC;
	faceList = bC(input, input->GetRequestedRegion(), m_Radius);

    typename itk::NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

	// Process each of the boundary faces.  These are N-d regions which border
	// the edge of the buffer.
	for (fit = faceList.begin(); fit != faceList.end(); ++fit) {

        bit = itk::ConstNeighborhoodIterator<InputImageType>(m_Radius, input, *fit);
		unsigned int neighborhoodSize = bit.Size();

		bit.OverrideBoundaryCondition(&cbc);
		bit.GoToBegin();

		while (!bit.IsAtEnd()) {
			bool isMaximum = true;
			InputPixelType centerValue = bit.GetCenterPixel(); //NumericTraits<InputRealType>::NonpositiveMin();
			if (centerValue > m_Threshold) {
				for (i = 0; i < neighborhoodSize; ++i) {
					InputPixelType tmp = bit.GetPixel(i);

					// if we find a neighbor with a larger value than the center, tthe center is not a maximum...
					if (tmp > centerValue) {
						isMaximum = false;
						break;
					}
				}

				if (isMaximum) {
                    typename TOutputContainer::AdherensJunctionVertexType::Pointer element =TOutputContainer::AdherensJunctionVertexType::New();
                    typename TOutputContainer::AdherensJunctionVertexType::PointType point;
                    this->GetInputImage()->TransformIndexToPhysicalPoint(bit.GetIndex(),point);
                    element->SetPosition(point);

                    m_LocalMaxima->AddVertex(element);
				}
			}
			++bit;

		}
	}
}

/**
 * Standard "PrintSelf" method
 */
template <class TInputImage,class TOutputContainer>
void
LocalMaximaImageFilter< TInputImage,TOutputContainer>
::PrintSelf(
  std::ostream& os,
  itk::Indent indent) const
{

  os << indent << "Radius: " << m_Radius << std::endl;
  os << indent << "Threshold: " << m_Threshold << std::endl;

}




#endif
