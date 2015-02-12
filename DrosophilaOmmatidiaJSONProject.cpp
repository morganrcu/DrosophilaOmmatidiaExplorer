#include "DrosophilaOmmatidiaJSONProject.h"
#include <vtk_jsoncpp.h>
#include <sstream>
#include <iostream>
#include <fstream>
DrosophilaOmmatidiaJSONProject::DrosophilaOmmatidiaJSONProject(){
}

bool DrosophilaOmmatidiaJSONProject::Open(const std::string & projectPath){
    this->m_ProjectPath=projectPath;

    std::stringstream projectFileStream("");
    projectFileStream << m_ProjectPath << "/" << "project.json";
    std::ifstream projectFile(projectFileStream.str(),std::ifstream::in);

     Json::Reader reader;
     Json::Value root;
     reader.parse(projectFile,root);
    this->m_FirstFrame= root["firstFrame"].asInt();
    this->m_LastFrame= root["lastFrame"].asInt();
    this->m_NumFrames= m_LastFrame - m_FirstFrame +1;

    return true;
}

typename DrosophilaOmmatidiaJSONProject::MotionImageType::Pointer
DrosophilaOmmatidiaJSONProject::GetMotionImage(int frame){

    typename MotionImageType::Pointer motionImage = MotionImageType::New();

    typename MotionImageType::RegionType region;

    typename MotionImageType::SpacingType spacing;
    spacing.Fill(1.0);
    motionImage->SetSpacing(spacing);

    typename MotionImageType::PointType origin;
    origin.Fill(0.0);
    motionImage->SetOrigin(origin);

    region.SetIndex(0,0);
    region.SetIndex(1,0);
    region.SetIndex(2,0);
    region.SetSize(0,100);
    region.SetSize(1,100);
    region.SetSize(2,100);

    motionImage->SetRegions(region);
    motionImage->Allocate();
    motionImage->FillBuffer(itk::NumericTraits<MotionImageType::PixelType>::OneValue());
    return motionImage.GetPointer();
}
