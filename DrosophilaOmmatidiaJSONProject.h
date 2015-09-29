#ifndef DROSOPHILAOMMATIDIAJSONPROJECT_H
#define DROSOPHILAOMMATIDIAJSONPROJECT_H

#include <itkImage.h>
#include <string>
#include <itkSymmetricSecondRankTensor.h>
#include <AdherensJunctionVertex.h>
#include <AdherensJunctionVerticesContainer.h>
#include <AJCorrespondenceSet.h>
#include <AJGraph.h>
#include <CellGraph.h>
#include <AJVertex.h>
#include <AJEdge.h>
#include <Cell.h>
#include <itkArray.h>
#include <FeatureContainer.h>
//#include <CellCorrespondence.h>
#include <OmmatidiaTissue.h>
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



    typedef OmmatidiaTissue<3> TissueType;
    typedef OmmatidiaTissue<3>::AJGraphType AdherensJunctionGraphType;
    typedef OmmatidiaTissue<3>::CellGraphType CellGraphType;


    typedef AJCorrespondenceSet<TissueType> CorrespondenceSetType;
    typedef CorrespondenceSetType::AJCorrespondenceType AJCorrespondenceType;
    //typedef CellCorrespondences<typename CellGraphType::CellVertexHandler> CellCorrespondenceType;

    typedef OriginalImageType::SpacingType SpacingType;

    SpacingType m_Spacing;



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

     OriginalImageType::Pointer GetOriginalImage(int frame);

     DeconvolutedImageType::Pointer GetDeconvolutedImage(int frame);

     MotionImageType::Pointer GetMotionImage(int frame);

     HessianImageType::Pointer GetHessianImage(int frame);

     PlatenessImageType::Pointer GetPlatenessImage(int frame);

     MolecularImageType::Pointer GetMolecularImage(int frame);


     VertexnessImageType::Pointer GetVertexnessImage(int frame);
    void SetVertexnessImage(int frame, VertexnessImageType::Pointer & vertexnessImage);

    void StoreAJGraph(const std::string & fileName,const  AdherensJunctionGraphType::Pointer & ajVertices);
    void SetAJGraph(int frame,const  AdherensJunctionGraphType::Pointer & ajVertices);
    void SetTissueDescriptor(int frame,const TissueType::Pointer & cellGraph);

    CorrespondenceSetType GetCorrespondences(int frame0,int frame1);
    void SetCorrespondences(int frame0,int frame1,const  CorrespondenceSetType & correspondences);

    //typename CellCorrespondenceType::Pointer GetCellCorrespondences(int frame0,int frame1);
    //void SetCellCorrespondences(int frame0,int frame1,const typename CellCorrespondenceType::Pointer & correspondences);

    //void SetVertexMolecularDistribution(int frame,const VertexMolecularFeatureMapType::Pointer & vertexMolecularDistribution);
    //typename VertexMolecularFeatureMapType::Pointer GetVertexMolecularDistribution(int frame);

    //void SetEdgeMolecularDistribution(int frame, const EdgeMolecularFeatureMapType::Pointer &edgeMolecularDistribution, const DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & graph);
    //EdgeMolecularFeatureMapType::Pointer GetEdgeMolecularDistribution(int frame,const typename AdherensJunctionGraphType::Pointer & ajGraph);

    AdherensJunctionGraphType::Pointer LoadAJGraph(const std::string & fileName);
    AdherensJunctionGraphType::Pointer GetAJGraph(int frame);

    TissueType::Pointer GetTissueDescriptor(int frame);


    bool IsOriginalImage(int frame);

    bool IsDeconvolutedImage(int frame);

    bool IsVertexnessImage(int frame);
    bool IsPlatenessImage(int frame);

    bool IsAJVertices(int frame);

    bool IsMolecularImage(int frame);

    bool IsTissueDescriptor(int frame);

private:
    bool IsImage(const std::string & name);
    template<class T> void ReadFrame(typename T::Pointer & result,const std::string & name);

    template<class T> void WriteFrame(const typename T::Pointer & image,const std::string & name);

    std::string m_ProjectPath;

    int m_FirstFrame;
    int m_LastFrame;
    int m_Level;
    unsigned int m_NumFrames;
};

#endif // DROSOPHILAOMMATIDIAJSONPROJECT_H
