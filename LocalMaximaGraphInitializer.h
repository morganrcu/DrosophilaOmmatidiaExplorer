#ifndef LOCALMAXIMAGRAPHINITIALIZER_H
#define LOCALMAXIMAGRAPHINITIALIZER_H

#include "itkImage.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkMacro.h"

#include <vector>

/** \class LocalMaximaGraphInitializer
 * \brief Calculates the Local Maximum values and puts them in a point-set/ mesh.
 *
 * This filter finds the local maxima of an image. The result is returned as a PointSet.
 * Additionally a binary image, with 0 for background, and 1 for the points can be retrieved.
 *
 * This filter assumes a pixel must be larger or equal all of it's neighbors to be a maximum.
 * The neighborhood size can be set by setting the Radius. The second parameter is a threshold.
 * Only maxima above the threshold are selected.
 *
 * TODO: allow user to set which information should be saved in the point-set (i.e. index or physical point, data)
 *
 *
 * \sa Image
 * \sa Neighborhood
 * \sa NeighborhoodOperator
 * \sa NeighborhoodIterator
 *
 * \ingroup IntensityImageFilters
 */
template <class TInputImage,class TOutputGraph>
class ITK_EXPORT LocalMaximaGraphInitializer : public itk::Object
{
public:


  /** Run-time type information (and related methods). */
  itkTypeMacro(LocalMaximaGraphInitializer, Object)


  /** Extract dimension from input and output image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Some typedefs associated with the input images. */
  typedef TInputImage                               InputImageType;
  typedef typename InputImageType::Pointer          InputImagePointer;
  typedef typename InputImageType::ConstPointer     InputImageConstPointer;
  typedef typename InputImageType::RegionType       InputImageRegionType;
  typedef itk::ImageRegionConstIteratorWithIndex<InputImageType> InputImageIterator;


  /** Image typedef support. */
  typedef typename InputImageType::PixelType InputPixelType;



  typedef typename InputImageType::SizeType InputSizeType;
  typedef typename InputImageType::IndexType InputIndexType;




  /** Standard class typedefs. */
  typedef LocalMaximaGraphInitializer Self;
  typedef itk::ProcessObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self)

  /** Set the radius of the neighborhood used to compute the mean. */
  itkSetMacro(Radius, InputSizeType)
  /** Get the radius of the neighborhood used to compute the mean */
  itkGetConstReferenceMacro(Radius, InputSizeType)

  /** Set the radius of the neighborhood used to compute the mean. */
  itkSetMacro(Threshold, InputPixelType)
  /** Get the radius of the neighborhood used to compute the mean */
  itkGetConstReferenceMacro(Threshold, InputPixelType)


  itkGetObjectMacro(InputImage,InputImageType)
  itkSetObjectMacro(InputImage,InputImageType)

  itkGetObjectMacro(OutputGraph,TOutputGraph)
  virtual void  Compute ();
protected:
  LocalMaximaGraphInitializer();
  virtual ~LocalMaximaGraphInitializer() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const;


private:
  LocalMaximaGraphInitializer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  InputSizeType m_Radius;
  InputPixelType m_Threshold;

  typename InputImageType::Pointer m_InputImage;

  typename TOutputGraph::Pointer m_OutputGraph;

};

#include "LocalMaximaGraphInitializer.hxx"



#endif // LOCALMAXIMAGRAPHINITIALIZER_H
