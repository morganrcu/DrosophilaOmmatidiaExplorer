#ifndef DROSOPHILAOMMATIDIAJSONPROJECT_H
#define DROSOPHILAOMMATIDIAJSONPROJECT_H

#include <itkImage.h>
#include <string>
/**
 * @brief The DrosophilaOmmatidiaJSONProject class encapsulates the IO operations with the filesystem
 */

class DrosophilaOmmatidiaJSONProject
{

public:

    /**
     * @brief OriginalImage type of the original volumes prior to deconvolution and motion estimation
     */
    typedef itk::Image<double,3> OriginalImageType;
    /**
     * @brief DeconvolutedImage type of the volumes after deconvolution and motion estimation
     */
    typedef itk::Image<double,3> DeconvolutedImageType;
    /**
     * @brief MotionImage type of the motion images after deconvolution
     */
    typedef itk::Image<itk::Vector<double,3> ,3> MotionImageType;

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


    inline int GetNumFrames(){
        return m_NumFrames;
    }

    typename OriginalImageType::Pointer GetOriginalImage(int frame);
    typename DeconvolutedImageType::Pointer GetDeconvolutedImage(int frame);
    typename MotionImageType::Pointer GetMotionImage(int frame);



private:

    std::string m_ProjectPath;

    int m_FirstFrame;
    int m_LastFrame;
    unsigned int m_NumFrames;
};

#endif // DROSOPHILAOMMATIDIAJSONPROJECT_H
