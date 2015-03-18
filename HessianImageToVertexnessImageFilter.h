#ifndef HESSIANIMAGETOVERTEXNESSIMAGEFILTER_H
#define HESSIANIMAGETOVERTEXNESSIMAGEFILTER_H
#include <itkUnaryFunctorImageFilter.h>

template< class TensorType> class VertexnessFunctor{
public:
    typedef typename TensorType::ComponentType ValueType;
    itkStaticConstMacro(Dimension, unsigned int, TensorType::Dimension);

    VertexnessFunctor(){
        m_Alpha=0.5;
        m_Beta=0.5;
        m_Gamma=8;
        m_C = 10e-6;
    }
    bool operator!=( const VertexnessFunctor & other ) const
        {
        return !(*this == other);
        }
    bool operator==( const VertexnessFunctor & other ) const
        {
        return true;
        }
    ValueType operator()(const TensorType  &  tensor)	{

        typename TensorType::EigenValuesArrayType eigenValue;

        tensor.ComputeEigenValues(eigenValue);

        double result = 0;
        // Find the smallest eigenvalue
        double smallest = vnl_math_abs(eigenValue[0]);
        double Lambda1 = eigenValue[0];
        for (unsigned int i = 1; i <= 2; i++) {
            if (vnl_math_abs(eigenValue[i]) < smallest) {
                Lambda1 = eigenValue[i];
                smallest = vnl_math_abs(eigenValue[i]);
            }
        }

        // Find the largest eigenvalue
        double largest = vnl_math_abs(eigenValue[0]);
        double Lambda3 = eigenValue[0];

        for (unsigned int i = 1; i <= 2; i++) {
            if (vnl_math_abs(eigenValue[i] > largest)) {
                Lambda3 = eigenValue[i];
                largest = vnl_math_abs(eigenValue[i]);
            }
        }

        //  find Lambda2 so that |Lambda1| < |Lambda2| < |Lambda3|
        double Lambda2 = eigenValue[0];

        for (unsigned int i = 0; i <= 2; i++) {
            if (eigenValue[i] != Lambda1 && eigenValue[i] != Lambda3) {
                Lambda2 = eigenValue[i];
                break;
            }
        }
        if(Lambda3>0 || Lambda2>0 || Lambda1>0){
        	return 0;
        }

        //if(eigenValue[0]>0 ||eigenValue[2]>0 || eigenValue[2]>0){
        //	return 0;
        //}else{
        	//return std::sqrt(eigenValue[0]* eigenValue[0] + eigenValue[1]*eigenValue[1] + eigenValue[2]* eigenValue[2]);
        	//return std::fabs(Lambda1)+ std::fabs(Lambda2) +std::fabs(Lambda3);
        //return std::fabs(Lambda1)+ std::fabs(Lambda2) +std::fabs(Lambda3);
        	//}
        //std::fabs(Lambda3)+ std::fabs(Lambda2) +std::fabs(Lambda3);
        return (std::fabs(Lambda1)/std::sqrt(Lambda3*Lambda2))*std::sqrt(Lambda1*Lambda1 + Lambda2*Lambda2 + Lambda3*Lambda3);

#if 0
        //




            if (Lambda3 >= 0.0 || Lambda2 >= 0.0 || Lambda1 >= 0.0 ) {
                return 0;
            } else {
                double Lambda1Sqr = vnl_math_sqr(Lambda1);
                double Lambda2Sqr = vnl_math_sqr(Lambda2);
                double Lambda3Sqr = vnl_math_sqr(Lambda3);

            	return vnl_math_sqrt(Lambda1Sqr+ Lambda2Sqr+ Lambda3Sqr);
#if 0
                double Lambda1Abs = vnl_math_abs(Lambda1);
                double Lambda2Abs = vnl_math_abs(Lambda2);
                double Lambda3Abs = vnl_math_abs(Lambda3);


                double AlphaSqr = vnl_math_sqr(m_Alpha);
                double BetaSqr = vnl_math_sqr(m_Beta);
                double GammaSqr = vnl_math_sqr(m_Gamma);

                double A {Lambda2Abs / Lambda3Abs};
                double B{Lambda1Abs / Lambda2Abs};
                double S = vcl_sqrt(Lambda1Sqr + Lambda2Sqr + Lambda3Sqr);

                double vesMeasure_1 = (1-(vcl_exp(
                        -0.5 * ((vnl_math_sqr(A)) / (AlphaSqr)))));

                double vesMeasure_2 = (1-vcl_exp(
                        -0.5 * ((vnl_math_sqr(B)) / (BetaSqr))));

                double vesMeasure_3 = (1
                        - vcl_exp(-1.0 * ((vnl_math_sqr(S)) / (2.0 * (GammaSqr)))));

                double vesMeasure_4 = vcl_exp(
                        -1.0 * (2.0 * vnl_math_sqr(m_C)) / (Lambda3Sqr));

                double vesselnessMeasure = vesMeasure_1 * vesMeasure_2
                        * vesMeasure_3 * vesMeasure_4;
                result=vesselnessMeasure;
#if 0
                if (m_ScalePlatenessMeasure) {
                    result = Lambda3Abs * vesselnessMeasure;
                }else{
                    result=vesselnessMeasure;
                }
#endif

                return result;
#endif


    }
#endif
    }
    typename TensorType::ComponentType m_Alpha;
    typename TensorType::ComponentType m_Beta;
    typename TensorType::ComponentType m_Gamma;
    typename TensorType::ComponentType m_C;
};

template<class TInputImage,class TOutputImage> class HessianImageToVertexnessImageFilter : public itk::UnaryFunctorImageFilter<TInputImage,TOutputImage,VertexnessFunctor<typename TInputImage::PixelType> >
{

public:
    typedef HessianImageToVertexnessImageFilter Self;
    typedef itk::UnaryFunctorImageFilter<TInputImage,TOutputImage,VertexnessFunctor<typename TInputImage::PixelType> > Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    itkNewMacro(Self)
    itkTypeMacro(Self,Superclass)
private:
    HessianImageToVertexnessImageFilter(){

    }
};

#endif // HESSIANIMAGETOVERTEXNESSIMAGEFILTER_H
