#ifndef HESSIANGRAPHINITIALIZER_H
#define HESSIANGRAPHINITIALIZER_H

#include <itkUnaryFunctorImageFilter.h>
#include <itkObject.h>
template<class THessian> class HessianEigenvaluesFunctor{

public:
    typedef typename THessian::EigenValuesArrayType ValueType;
    itkStaticConstMacro(Dimension, unsigned int, THessian::Dimension);

    HessianEigenvaluesFunctor(){

    }
    bool operator!=( const HessianEigenvaluesFunctor & other ) const            {
        return !(*this == other);
    }
    bool operator==( const HessianEigenvaluesFunctor & other ) const
    {
        return true;
    }
    ValueType operator()(const THessian  &  tensor)	{
        typename THessian::EigenValuesArrayType eigenValue;
        tensor.ComputeEigenValues(eigenValue);
        return eigenValue;

    }
};

template<class THessianImage,class TOutputGraph> class HessianGraphInitializer : public itk::Object{

    typedef itk::Image<typename THessianImage::PixelType::EigenValuesArrayType, THessianImage::ImageDimension> EigenValueImageType;

    typedef itk::UnaryFunctorImageFilter<THessianImage,EigenValueImageType, HessianEigenvaluesFunctor<typename THessianImage::PixelType> > EigenCalculatorType;


public:

    typedef HessianGraphInitializer Self;
    typedef itk::Object Superclass;
    typedef itk::SmartPointer<Self> Pointer;

    itkNewMacro(Self)


    itkGetObjectMacro(InputImage,THessianImage)
    itkSetObjectMacro(InputImage,THessianImage)
    itkSetObjectMacro(OutputGraph,TOutputGraph)
    itkGetObjectMacro(OutputGraph,TOutputGraph)

    virtual void Compute(){


        this->m_OutputGraph= TOutputGraph::New();

        typename EigenCalculatorType::Pointer eigenCalculator = EigenCalculatorType::New();
        eigenCalculator->SetInput(this->GetInputImage());
        eigenCalculator->Update();


        typename EigenValueImageType::Pointer eigenImage=eigenCalculator->GetOutput();

        typename EigenValueImageType::IndexType position;
        typename EigenValueImageType::SizeType size = eigenImage->GetLargestPossibleRegion().GetSize();

        for (position[0] = 0; position[0] < size[0]; position[0]++) {
            for (position[1] =  0; position[1] < size[1]; position[1]++) {
                for (position[2] = 0; position[2]  < size[2]; position[2]++) {

                    typename EigenValueImageType::PixelType centerValue = eigenImage->GetPixel(position);

                    if(centerValue[0]>=0 || centerValue[1]>=0 || centerValue[2]>=0) continue;

                    double prod= fabs(centerValue[0])*fabs(centerValue[1])*fabs(centerValue[2]);
                    double sum = fabs(centerValue[0])+fabs(centerValue[1])+fabs(centerValue[2]);
                    std::cout << prod << " " << sum << std::endl;
                    if(sum<1e-3 || prod < 1e-5) continue;
                    //std::cout << pow(std::fabs(centerValue[0])*std::fabs(centerValue[1])*std::fabs(centerValue[2]),1.0/3)<<"<"<< 100 <<std::endl;

                    bool max = true;
                    typename EigenValueImageType::OffsetType offset;

                    for (offset[0]  = -1; offset[0] <= 1 && max; offset[0]++) {
                        for (offset[1] = -1; offset[1] <= 1 && max; offset[1]++) {
                            for (offset[2] =  -1; offset[2]  <= 1 && max; offset[2]++) {
                                if (offset[0] == 0 && offset[1] == 0 && offset[2] == 0 ) continue;

                                    typename EigenValueImageType::IndexType neigh = position  + offset;

                                    if(!eigenImage->GetBufferedRegion().IsInside(neigh)) continue;


                                    typename EigenValueImageType::PixelType neighValue = eigenImage->GetPixel(neigh);
                                    if(neighValue[0]>=0 || neighValue[1]>=0 || neighValue[2]>=0) continue;

                                    double prodNeigh= fabs(neighValue[0])*fabs(neighValue[1])*fabs(neighValue[2]);
                                    double sumNeigh = fabs(neighValue[0])+fabs(neighValue[1])+fabs(neighValue[2]);

                                    if(prodNeigh>prod || sumNeigh > sum){
                                        max = false;
                                    }
                            }
                        }
                    }

                    if(max){

                        typename TOutputGraph::AJVertexType::Pointer vertex =TOutputGraph::AJVertexType::New();

                        typename TOutputGraph::AJVertexType::PointType point;

                        eigenImage->TransformIndexToPhysicalPoint(position,point);

                        vertex->SetPosition(point);

                        m_OutputGraph->AddAJVertex(vertex);
                    }
                }
            }
        }
    }

private:

    typename THessianImage::Pointer m_InputImage;

    typename TOutputGraph::Pointer m_OutputGraph;
};

#endif // HESSIANGRAPHINITIALIZER_H
