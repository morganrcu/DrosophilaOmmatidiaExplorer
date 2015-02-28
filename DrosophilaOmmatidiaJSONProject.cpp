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
    writer->SetInput(image);
    writer->Update();
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

typename DrosophilaOmmatidiaJSONProject::PlatenessImageType::Pointer DrosophilaOmmatidiaJSONProject::GetPlatenessImage(int frame){

    typename PlatenessImageType::Pointer platenessImage{};

    std::stringstream buffer("");
    buffer << "plateness_T" << m_FirstFrame+frame <<".mha";

    std::string fileName=buffer.str();
    this->ReadFrame<PlatenessImageType>(platenessImage,fileName);
    return platenessImage;
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
typename DrosophilaOmmatidiaJSONProject::VertexMolecularFeatureMapType DrosophilaOmmatidiaJSONProject::GetVertexMolecularDistribution(int frame){


    Json::Reader reader;
    Json::Value root;

    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "verticesMolecules" << m_FirstFrame+frame << ".json";

    std::string ajVerticesFile;
    fileNameStream >> ajVerticesFile;

    std::cout << ajVerticesFile << std::endl;

    std::ifstream jsonAJVerticesFile(ajVerticesFile.c_str());
    reader.parse(jsonAJVerticesFile, root);

    typename AdherensJunctionGraphType::Pointer ajGraph = AdherensJunctionGraphType::New();

    int numvertices = root["Molecules"].size();

    VertexMolecularFeatureMapType descriptors;
    for (int i = 0; i < numvertices; i++) {

        typename AdherensJunctionGraphType::AJVertexHandler vertex =root["Molecules"][i]["vertex"].asUInt64();

        unsigned int length=root["Molecules"][i]["length"].asUInt();
        itk::Array<double>  descriptor(length);

        for(int l=0;l<length;l++){
            descriptor[l]=root["Molecules"][i]["descriptor"][l].asDouble();
        }

        descriptors[vertex]=descriptor;
    }
    return descriptors;
}

typename DrosophilaOmmatidiaJSONProject::EdgeMolecularFeatureMapType DrosophilaOmmatidiaJSONProject::GetEdgeMolecularDistribution(int frame,const typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & ajGraph){

    Json::Reader reader;
    Json::Value root;

    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "edgesMolecules" << "_T" << m_FirstFrame+frame << ".json";
    std::string ajVerticesFile;
    fileNameStream >> ajVerticesFile;

    std::cout << ajVerticesFile << std::endl;

    std::ifstream jsonAJVerticesFile(ajVerticesFile.c_str());
    reader.parse(jsonAJVerticesFile, root);

    int numvertices = root["Molecules"].size();

    EdgeMolecularFeatureMapType descriptors;
    for (int i = 0; i < numvertices; i++) {


        typename AdherensJunctionGraphType::AJVertexHandler source =root["Molecules"][i]["source"].asUInt64();
        typename AdherensJunctionGraphType::AJVertexHandler target =root["Molecules"][i]["target"].asUInt64();

        typename AdherensJunctionGraphType::AJEdgeHandler edge = ajGraph->GetAJEdgeHandler(source,target);
        unsigned int length=root["Molecules"][i]["length"].asUInt();
        itk::Array<double>  descriptor(length);

        for(int l=0;l<length;l++){
            descriptor[l]=root["Molecules"][i]["descriptor"][l].asDouble();
        }

        descriptors[edge]=descriptor;
    }
    return descriptors;


}


void DrosophilaOmmatidiaJSONProject::SetVertexMolecularDistribution(int frame, const VertexMolecularFeatureMapType & vertexMolecularDistribution){


    Json::StyledWriter writer;
    Json::Value root;
    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "verticesMolecules" << "_T" << m_FirstFrame+frame << ".json";

    std::string vertexStorageFile;
    fileNameStream >> vertexStorageFile;

    int i=0;

    for(auto elem : vertexMolecularDistribution){
        AdherensJunctionGraphType::AJVertexHandler vertex= elem.first;
        itk::Array<double> descriptor = elem.second;
        root["Molecules"][i]["vertex"]=static_cast<Json::UInt64>(vertex);
        root["Molecules"][i]["length"]=descriptor.Size();
        for(int l=0;l<descriptor.Size();l++){
            root["Molecules"][i]["descriptor"][l]=descriptor[l];
        }
    }

    std::string jsoncontent = writer.write(root);
    std::ofstream file(vertexStorageFile.c_str(),
            std::ofstream::out | std::ofstream::trunc);

    file << jsoncontent;

    file.close();

}

void DrosophilaOmmatidiaJSONProject::SetEdgeMolecularDistribution(int frame, const EdgeMolecularFeatureMapType &edgeMolecularDistribution, const DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & graph){

    Json::StyledWriter writer;
    Json::Value root;
    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "edgesMolecules" << "_T" << m_FirstFrame+frame << ".json";

    std::string vertexStorageFile;
    fileNameStream >> vertexStorageFile;

    int i=0;

    for(auto elem : edgeMolecularDistribution){
        AdherensJunctionGraphType::AJEdgeHandler edge= elem.first;
        itk::Array<double> descriptor = elem.second;
        root["Molecules"][i]["source"]=(Json::UInt64)graph->GetAJEdgeSource(edge);
        root["Molecules"][i]["target"]=(Json::UInt64)graph->GetAJEdgeTarget(edge);
        root["Molecules"][i]["length"]=descriptor.Size();
        for(int l=0;l<descriptor.Size();l++){
            root["Molecules"][i]["descriptor"][l]=descriptor[l];
        }
    }

    std::string jsoncontent = writer.write(root);
    std::ofstream file(vertexStorageFile.c_str(),
            std::ofstream::out | std::ofstream::trunc);

    file << jsoncontent;

    file.close();

}

bool DrosophilaOmmatidiaJSONProject::IsMolecularImage(int frame){
    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "actin" << "_T" << m_FirstFrame+frame << ".mha";

    std::ifstream file(fileNameStream.str());
    return file.is_open();
}

typename DrosophilaOmmatidiaJSONProject::MolecularImageType::Pointer DrosophilaOmmatidiaJSONProject::GetMolecularImage(int frame){

    typename MolecularImageType::Pointer molecularImage{};

    std::stringstream buffer("");
    buffer << "actin_T" << m_FirstFrame+frame <<".mha";

    std::string fileName=buffer.str();
    this->ReadFrame<MolecularImageType>(molecularImage,fileName);
    return molecularImage;
}

typename DrosophilaOmmatidiaJSONProject::HessianImageType::Pointer DrosophilaOmmatidiaJSONProject::GetHessianImage(int frame){
    typename HessianImageType::Pointer hessianImage{};
    std::stringstream buffer("");
    buffer << "hessian_T" << m_FirstFrame+frame << ".mha";
    std::string fileName = buffer.str();
    this->ReadFrame<HessianImageType>(hessianImage,fileName);
    return hessianImage;
}


void DrosophilaOmmatidiaJSONProject::SetVertexnessImage(int frame, typename VertexnessImageType::Pointer & vertexnessImage){

    std::stringstream buffer("");
    buffer << "vertexness_T" << m_FirstFrame+frame << ".mha";
    std::string fileName = buffer.str();
    this->WriteFrame<VertexnessImageType>(vertexnessImage,fileName);

}

bool DrosophilaOmmatidiaJSONProject::IsAJVertices(int frame){
    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "vertices" << "_T" << m_FirstFrame+frame << ".json";

    std::ifstream file(fileNameStream.str());
    return file.is_open();

}

void DrosophilaOmmatidiaJSONProject::SetAJGraph(int frame,const typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & ajVertices){
    Json::StyledWriter writer;
    Json::Value root;
    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "vertices" << "_T" << m_FirstFrame+frame << ".json";

    std::string vertexStorageFile;
    fileNameStream >> vertexStorageFile;

    int i=0;
    for(auto it =ajVertices->VerticesBegin();it!=ajVertices->VerticesEnd();it++){

        typename AdherensJunctionGraphType::AJVertexType::PointType position = ajVertices->GetAJVertex((*it))->GetPosition();
        root["Vertices"][i]["x"] =  position[0];
        root["Vertices"][i]["y"] =  position[1];
        root["Vertices"][i]["z"] =  position[2];
        i++;
    }
    i=0;
    for(auto it=ajVertices->EdgesBegin();it!=ajVertices->EdgesEnd();it++){
        typename AdherensJunctionGraphType::AJVertexHandler source,target;
        source = ajVertices->GetAJEdgeSource(*it);
        target = ajVertices->GetAJEdgeTarget(*it);

        root["Edges"][i]["s"] =  (Json::UInt64)source;
        root["Edges"][i]["t"] =  (Json::UInt64)target;

        i++;
    }

    std::string jsoncontent = writer.write(root);
    std::ofstream file(vertexStorageFile.c_str(),
            std::ofstream::out | std::ofstream::trunc);

    file << jsoncontent;

    file.close();
}

typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer DrosophilaOmmatidiaJSONProject::GetAJGraph(int frame){

    Json::Reader reader;
    Json::Value root;

    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "vertices_T" << m_FirstFrame+frame << ".json";

    std::string ajVerticesFile;
    fileNameStream >> ajVerticesFile;

        std::cout << ajVerticesFile << std::endl;

        std::ifstream jsonAJVerticesFile(ajVerticesFile.c_str());
        reader.parse(jsonAJVerticesFile, root);

        typename AdherensJunctionGraphType::Pointer ajGraph = AdherensJunctionGraphType::New();

        int numvertices = root["Vertices"].size();

        for (int i = 0; i < numvertices; i++) {
            typename AdherensJunctionGraphType::AJVertexType::Pointer  newVertex =
                    AdherensJunctionGraphType::AJVertexType::New();

            typename AdherensJunctionGraphType::AJVertexType::PointType position;

            position[0] = root["Vertices"][i]["x"].asDouble();
            position[1] = root["Vertices"][i]["y"].asDouble();
            position[2] = root["Vertices"][i]["z"].asDouble();
            newVertex->SetPosition(position);

            ajGraph->AddAJVertex(newVertex);
        }

        int numedges=root["Edges"].size();

        for (int i = 0; i < numedges; i++) {
            typename AdherensJunctionGraphType::AJVertexHandler source,target;

            source = root["Edges"][i]["s"].asUInt64();
            target = root["Edges"][i]["t"].asUInt64();

            ajGraph->AddAJEdge(source,target);
        }

        return ajGraph;

}
