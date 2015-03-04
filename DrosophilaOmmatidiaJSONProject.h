#ifndef DROSOPHILAOMMATIDIAJSONPROJECT_H
#define DROSOPHILAOMMATIDIAJSONPROJECT_H

#include <itkImage.h>
#include <string>
#include <itkSymmetricSecondRankTensor.h>
#include <AdherensJunctionVertex.h>
#include <AdherensJunctionVerticesContainer.h>
#include <AJGraph.h>
#include <AJVertex.h>
#include <AJEdge.h>
#include <itkArray.h>
#include <FeatureContainer.h>
/**
 * @brief The DrosophilaOmmatidiaJSONProject class encapsulates the IO operations with the filesystem
 */

class DrosophilaOmmatidiaJSONProject
{

public:

    /**
     * @brief OriginalImageType type of the original volumes prior to deconvolution and motion estimation
     */
    typedef itk::Image<double,3> OriginalImageType;
    /**
     * @brief DeconvolutedImageType type of the volumes after deconvolution and motion estimation
     */
    typedef itk::Image<double,3> DeconvolutedImageType;
    /**
     * @brief MotionImage type of the motion images after deconvolution
     */
    typedef itk::Image<itk::Vector<double,3> ,3> MotionImageType;

    /**
     * @brief HessianImageType type of the hessian images
     */
    typedef itk::Image<itk::SymmetricSecondRankTensor<double,3>,3> HessianImageType;

    /**
     * @brief VertexnessImageType type of the volumes after deconvolution and motion estimation
     */
    typedef itk::Image<double,3> VertexnessImageType;

    /**
     * @brief PlatenessImageType type of the volumes after deconvolution and motion estimation
     */
    typedef itk::Image<double,3> PlatenessImageType;


    /**
     * @brief PlatenessImageType type of the volumes after deconvolution and motion estimation
     */
    typedef itk::Image<double,3> MolecularImageType;


    //typedef AdherensJunctionVerticesContainer<AdherensJunctionVertex<3> > AdherensJunctionVerticesContainerType;

    typedef AJGraph<AJVertex,AJEdge> AdherensJunctionGraphType;





    /**
     * @brief DrosophilaOmmatidiaJSONProject Class constructor. Does nothing
     */
    DrosophilaOmmatidiaJSONProject();

    /**
     * @brief Open
     * @param fileName
     * @return true if the project was properly opened or false if there was a problem opening the project
     */
    bool Open(const std::string & fileName);


    inline int GetNumberOfFrames(){
        return m_NumFrames;
    }

    typename OriginalImageType::Pointer GetOriginalImage(int frame);

    typename DeconvolutedImageType::Pointer GetDeconvolutedImage(int frame);

    typename MotionImageType::Pointer GetMotionImage(int frame);

    typename HessianImageType::Pointer GetHessianImage(int frame);

    typename PlatenessImageType::Pointer GetPlatenessImage(int frame);

    typename MolecularImageType::Pointer GetMolecularImage(int frame);



    void SetVertexnessImage(int frame,typename VertexnessImageType::Pointer & vertexnessImage);

    void SetAJGraph(int frame,const typename AdherensJunctionGraphType::Pointer & ajVertices);

    //void SetVertexMolecularDistribution(int frame,const VertexMolecularFeatureMapType::Pointer & vertexMolecularDistribution);
    //typename VertexMolecularFeatureMapType::Pointer GetVertexMolecularDistribution(int frame);

    //void SetEdgeMolecularDistribution(int frame, const EdgeMolecularFeatureMapType::Pointer &edgeMolecularDistribution, const DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & graph);
    //EdgeMolecularFeatureMapType::Pointer GetEdgeMolecularDistribution(int frame,const typename AdherensJunctionGraphType::Pointer & ajGraph);

    typename AdherensJunctionGraphType::Pointer GetAJGraph(int frame);

    bool IsOriginalImage(int frame);

    bool IsDeconvolutedImage(int frame);

    bool IsVertexnessImage(int frame);
    bool IsPlatenessImage(int frame);

    bool IsAJVertices(int frame);\

    bool IsMolecularImage(int frame);


private:
    bool IsImage(const std::string & name);
    template<class T> void ReadFrame(typename T::Pointer & result,const std::string & name);

    template<class T> void WriteFrame(const typename T::Pointer & image,const std::string & name);

    std::string m_ProjectPath;

    int m_FirstFrame;
    int m_LastFrame;
    unsigned int m_NumFrames;
};

#endif // DROSOPHILAOMMATIDIAJSONPROJECT_H
