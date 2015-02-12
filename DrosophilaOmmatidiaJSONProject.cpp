#include "DrosophilaOmmatidiaJSONProject.h"
#include <vtk_jsoncpp.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
DrosophilaOmmatidiaJSONProject::DrosophilaOmmatidiaJSONProject(){
}

template<class T> void DrosophilaOmmatidiaJSONProject::ReadFrame(typename T::Pointer & result,const std::string & name){
    std::stringstream buffer("");
    buffer << this->m_ProjectPath << "/" <<name;
    typedef itk::ImageFileReader<T> ReaderType;

    typename ReaderType::Pointer reader = ReaderType::New();

    std::cout << buffer.str() << std::endl;
    reader->SetFileName(buffer.str());
    reader->Update();
    result = reader->GetOutput();
    assert(result);
    result->DisconnectPipeline();


}

template<class T> void DrosophilaOmmatidiaJSONProject::WriteFrame(const typename T::Pointer & image,const std::string & name){
    std::stringstream buffer("");
    buffer << this->m_ProjectPath << "/" <<name;
    typedef itk::ImageFileWriter<T> WriterType;

    typename WriterType::Pointer writer = WriterType::New();

    std::cout << buffer.str() << std::endl;
    writer->SetFileName(buffer.str());
    writer->Update();
    image = writer->GetOutput();
    assert(image);
    image->DisconnectPipeline();



}

bool DrosophilaOmmatidiaJSONProject::Open(const std::string & projectPath){
    this->m_ProjectPath=projectPath;

    std::stringstream projectFileStream("");
    projectFileStream << m_ProjectPath << "/" << "project.json";
    std::ifstream projectFile(projectFileStream.str(),std::ifstream::in);
    if(!projectFile.is_open()){
        return false;
    }

    Json::Reader reader;
    Json::Value root;
    reader.parse(projectFile,root);

    this->m_FirstFrame= root["firstFrame"].asInt();
    this->m_LastFrame= root["lastFrame"].asInt();
    this->m_NumFrames= m_LastFrame - m_FirstFrame +1;

    return true;
}

typename DrosophilaOmmatidiaJSONProject::OriginalImageType::Pointer DrosophilaOmmatidiaJSONProject::GetOriginalImage(int frame){

    typename OriginalImageType::Pointer originalImage{};

    std::stringstream buffer("");
    buffer << "original_T" << m_FirstFrame+frame <<".mha";

    std::string fileName=buffer.str();
    this->ReadFrame<OriginalImageType>(originalImage,fileName);
    return originalImage;
}

typename DrosophilaOmmatidiaJSONProject::DeconvolutedImageType::Pointer DrosophilaOmmatidiaJSONProject::GetDeconvolutedImage(int frame){

    typename DeconvolutedImageType::Pointer deconvolutedImage{};

    std::stringstream buffer("");
    buffer << "estimate_T" << m_FirstFrame+frame <<".mha";

    std::string fileName=buffer.str();
    this->ReadFrame<DeconvolutedImageType>(deconvolutedImage,fileName);
    return deconvolutedImage;
}


typename DrosophilaOmmatidiaJSONProject::MotionImageType::Pointer DrosophilaOmmatidiaJSONProject::GetMotionImage(int frame){

    typename MotionImageType::Pointer motionImage{};

    std::stringstream buffer("");
    buffer << "registration_T" << m_FirstFrame+frame << "_T" << m_FirstFrame+frame+1 <<".mha";

    std::string fileName=buffer.str();
    this->ReadFrame<MotionImageType>(motionImage,fileName);
    return motionImage;
}


