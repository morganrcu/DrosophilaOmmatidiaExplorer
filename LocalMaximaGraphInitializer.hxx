#ifndef LOCALMAXIMAGRAPHINITIALIZER_HXX
#define LOCALMAXIMAGRAPHINITIALIZER_HXX

#include "LocalMaximaGraphInitializer.h"

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

template <class TInputImage,class TOutputGraph> LocalMaximaGraphInitializer< TInputImage,TOutputGraph>::LocalMaximaGraphInitializer()
{

  this->m_Radius.Fill(1);
  this->m_Threshold = 0.005;

  this->m_OutputGraph = TOutputGraph::New();
}



template <class TInputImage,class TOutputGraph> void LocalMaximaGraphInitializer< TInputImage,TOutputGraph>::Compute() {


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


                    typename TOutputGraph::AJVertexType::Pointer vertex =TOutputGraph::AJVertexType::New();

                    typename TOutputGraph::AJVertexType::PointType point;
                    this->GetInputImage()->TransformIndexToPhysicalPoint(bit.GetIndex(),point);
                    vertex->SetPosition(point);

                    m_OutputGraph->AddAJVertex(vertex);
                }
            }
            ++bit;

        }
    }
}

/**
 * Standard "PrintSelf" method
 */
template <class TInputImage,class TOutputGraph>
void
LocalMaximaGraphInitializer< TInputImage,TOutputGraph>
::PrintSelf(
  std::ostream& os,
  itk::Indent indent) const
{

  os << indent << "Radius: " << m_Radius << std::endl;
  os << indent << "Threshold: " << m_Threshold << std::endl;

}





#endif // LOCALMAXIMAGRAPHINITIALIZER_HXX
