#include "DrosophilaOmmatidiaJSONProject.h"
#include <vtk_jsoncpp.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <FeatureContainer.h>
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
#if 0
typename DrosophilaOmmatidiaJSONProject::VertexMolecularFeatureMapType::Pointer DrosophilaOmmatidiaJSONProject::GetVertexMolecularDistribution(int frame){


}

typename DrosophilaOmmatidiaJSONProject::EdgeMolecularFeatureMapType::Pointer DrosophilaOmmatidiaJSONProject::GetEdgeMolecularDistribution(int frame,const typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & ajGraph){

    Json::Reader reader;
    Json::Value root;

    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "edgesMolecules" << "_T" << m_FirstFrame+frame << ".json";
    std::string ajVerticesFile;
    fileNameStream >> ajVerticesFile;

    std::cout << ajVerticesFile << std::endl;

    std::ifstream jsonAJVerticesFile(ajVerticesFile.c_str());
    reader.parse(jsonAJVerticesFile, root);

    int numedges = root["Molecules"].size();

    typename EdgeMolecularFeatureMapType::Pointer descriptors=EdgeMolecularFeatureMapType::New();
    for (int i = 0; i < numedges; i++) {


        typename AdherensJunctionGraphType::AJVertexHandler source =root["Molecules"][i]["source"].asUInt64();
        typename AdherensJunctionGraphType::AJVertexHandler target =root["Molecules"][i]["target"].asUInt64();

        //typename AdherensJunctionGraphType::AJEdgeHandler edge = ajGraph->GetAJEdgeHandler(source,target);
        unsigned int length=root["Molecules"][i]["length"].asUInt();
        typedef itk::Array<double> DescriptorType;
        DescriptorType  distribution(length);


        for(int l=0;l<length;l++){
        	distribution[l]=root["Molecules"][i]["descriptor"][l].asDouble();
        }
        unsigned long edgeNumber=ajGraph->GetAJEdgeNumber(source,target);
        std::cout << edgeNumber << std::endl;
        typename FeatureDescriptor< DescriptorType>::Pointer descriptor = FeatureDescriptor< DescriptorType>::New();
        descriptor->SetValue(distribution);
        descriptors->AddFeature(edgeNumber,descriptor);

    }
#if 0
    for(auto it = descriptors.begin();it!=descriptors.end();it++){
    	//std::cout << it->first << " " << it->second << std::endl;
    }
#endif
    return descriptors;


}


void DrosophilaOmmatidiaJSONProject::SetVertexMolecularDistribution(int frame, const VertexMolecularFeatureMapType::Pointer & vertexMolecularDistribution){


    Json::StyledWriter writer;
    Json::Value root;
    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "verticesMolecules" << "_T" << m_FirstFrame+frame << ".json";

    std::string vertexStorageFile;
    fileNameStream >> vertexStorageFile;

    int i=0;

    for(auto it = vertexMolecularDistribution->FeaturesBegin();it!=vertexMolecularDistribution->FeaturesEnd();it++){
        AdherensJunctionGraphType::AJVertexHandler vertex= it->first;
        typedef itk::Array<double> DistributionType;
        typename FeatureDescriptor< DistributionType>::Pointer descriptor =  it->second;
        root["Molecules"][i]["vertex"]=static_cast<Json::UInt64>(vertex);
        root["Molecules"][i]["length"]=static_cast<Json::UInt64>(descriptor->GetValue().size());
        for(int l=0;l<descriptor->GetValue().size();l++){
            root["Molecules"][i]["descriptor"][l]=descriptor->GetValue()[l];
        }
        i++;
    }

    std::string jsoncontent = writer.write(root);
    std::ofstream file(vertexStorageFile.c_str(),
            std::ofstream::out | std::ofstream::trunc);

    file << jsoncontent;

    file.close();

}

void DrosophilaOmmatidiaJSONProject::SetEdgeMolecularDistribution(int frame, const EdgeMolecularFeatureMapType::Pointer & edgeMolecularDistribution, const DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & graph){

    Json::StyledWriter writer;
    Json::Value root;
    std::stringstream fileNameStream;
    fileNameStream << this->m_ProjectPath << "/" << "edgesMolecules" << "_T" << m_FirstFrame+frame << ".json";

    std::string vertexStorageFile;
    fileNameStream >> vertexStorageFile;

    int i=0;

    for(auto it = edgeMolecularDistribution->FeaturesBegin();it!=edgeMolecularDistribution->FeaturesEnd();it++){

    	AdherensJunctionGraphType::AJEdgeHandler edge= graph->GetAJEdgeHandelerFromNumber(it->first);
        typedef itk::Array<double> DistributionType;
        typename FeatureDescriptor< DistributionType>::Pointer descriptor =  it->second;


        root["Molecules"][i]["source"]=(Json::UInt64)graph->GetAJEdgeSource(edge);
        root["Molecules"][i]["target"]=(Json::UInt64)graph->GetAJEdgeTarget(edge);
        root["Molecules"][i]["length"]=(Json::UInt64)descriptor->GetValue().size();
        for(int l=0;l<descriptor->GetValue().size();l++){
            root["Molecules"][i]["descriptor"][l]=descriptor->GetValue()[l];
        }
        i++;
    }

    std::string jsoncontent = writer.write(root);
    std::ofstream file(vertexStorageFile.c_str(),
            std::ofstream::out | std::ofstream::trunc);

    file << jsoncontent;

    file.close();

}
#endif

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



typename DrosophilaOmmatidiaJSONProject::VertexnessImageType::Pointer DrosophilaOmmatidiaJSONProject::GetVertexnessImage(int frame){

    typename VertexnessImageType::Pointer vertexnessImage{};

    std::stringstream buffer("");
    buffer << "vertexness_T" << m_FirstFrame+frame <<".mha";

    std::string fileName=buffer.str();
    this->ReadFrame<VertexnessImageType>(vertexnessImage,fileName);
    return vertexnessImage;
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
        typename AdherensJunctionGraphType::AJVertexType::VectorType velocity = ajVertices->GetAJVertex((*it))->GetVelocity();
        root["Vertices"][i]["x"] =  position[0];
        root["Vertices"][i]["y"] =  position[1];
        root["Vertices"][i]["z"] =  position[2];

        root["Vertices"][i]["dx"]=velocity[0];
        root["Vertices"][i]["dy"]=velocity[1];
        root["Vertices"][i]["dz"]=velocity[2];

        itk::Array<double > descriptor=ajVertices->GetAJVertex((*it))->GetDescriptor();
        for(int l=0;l<descriptor.size();l++){
        	root["Vertices"][i]["descriptor"][l]=descriptor[l];
        }
        i++;
    }
    i=0;
    for(auto it=ajVertices->EdgesBegin();it!=ajVertices->EdgesEnd();it++){
        typename AdherensJunctionGraphType::AJVertexHandler source,target;
        source = ajVertices->GetAJEdgeSource(*it);
        target = ajVertices->GetAJEdgeTarget(*it);

        root["Edges"][i]["s"] =  (Json::UInt64)source;
        root["Edges"][i]["t"] =  (Json::UInt64)target;

        itk::Array<double > descriptor=ajVertices->GetAJEdge((*it))->GetDescriptor();
        for(int l=0;l<descriptor.size();l++){
        	root["Edges"][i]["descriptor"][l]=descriptor[l];
        }

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
            typename AdherensJunctionGraphType::AJVertexType::VectorType velocity;
            if(root["Vertices"][i].isMember("dx")){
            	velocity[0] = root["Vertices"][i]["dx"].asDouble();
            	velocity[1] = root["Vertices"][i]["dy"].asDouble();
            	velocity[2] = root["Vertices"][i]["dz"].asDouble();
            	newVertex->SetVelocity(velocity);
            }




            unsigned int length=root["Vertices"][i]["descriptor"].size();
            typedef itk::Array<double> DescriptorType;
            DescriptorType  distribution(length);

            for(int l=0;l<length;l++){
            	distribution[l]=root["Vertices"][i]["descriptor"][l].asDouble();
            }

            newVertex->SetDescriptor(distribution);

            ajGraph->AddAJVertex(newVertex);
        }

        int numedges=root["Edges"].size();

        for (int i = 0; i < numedges; i++) {
            typename AdherensJunctionGraphType::AJVertexHandler source,target;

            source = root["Edges"][i]["s"].asUInt64();
            target = root["Edges"][i]["t"].asUInt64();

            unsigned int length=root["Edges"][i]["descriptor"].size();
            typedef itk::Array<double> DescriptorType;
            DescriptorType  distribution(length);
            for(int l=0;l<length;l++){
            	distribution[l]=root["Edges"][i]["descriptor"][l].asDouble();
            }
            auto edgeHandler=ajGraph->AddAJEdge(source,target);
            typename AJEdge::Pointer edge=ajGraph->GetAJEdge(edgeHandler);
            edge->SetDescriptor(distribution);
        }

        return ajGraph;

#if 0

         std::stringstream fileNameStream;
        fileNameStream << this->m_ProjectPath << "/" << "verticesMolecules_T" << m_FirstFrame+frame << ".json";

        std::string ajVerticesFile;
        fileNameStream >> ajVerticesFile;

        std::cout << ajVerticesFile << std::endl;

        std::ifstream jsonAJVerticesFile(ajVerticesFile.c_str());
        reader.parse(jsonAJVerticesFile, root);


        int numvertices = root["Molecules"].size();

        VertexMolecularFeatureMapType::Pointer descriptors=VertexMolecularFeatureMapType::New();
        for (int i = 0; i < numvertices; i++) {

            typename AdherensJunctionGraphType::AJVertexHandler vertex =root["Molecules"][i]["vertex"].asUInt64();



            typename FeatureDescriptor< DescriptorType>::Pointer descriptor = FeatureDescriptor< DescriptorType>::New();
            descriptor->SetValue(distribution);
            descriptors->AddFeature(vertex,descriptor);
        }
        return descriptors;
#endif
}
