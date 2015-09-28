#include "DrosophilaOmmatidiaJSONProject.h"
#include <vtk_jsoncpp.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <FeatureContainer.h>
#include <itkMetaImageIO.h>
DrosophilaOmmatidiaJSONProject::DrosophilaOmmatidiaJSONProject() {
	m_Level = 3;
}

template<class T> void DrosophilaOmmatidiaJSONProject::ReadFrame(typename T::Pointer & result, const std::string & name) {
	std::stringstream buffer("");
	buffer << this->m_ProjectPath << "/" << name;
	typedef itk::ImageFileReader<T> ReaderType;

	typename ReaderType::Pointer reader = ReaderType::New();

	typedef itk::MetaImageIO ImageIOType;
	typename ImageIOType::Pointer imageIO = ImageIOType::New();
	reader->SetImageIO(imageIO);

	std::cout << buffer.str() << std::endl;
	reader->SetFileName(buffer.str());
	reader->Update();
	result = reader->GetOutput();
	assert(result);
	result->DisconnectPipeline();

}

template<class T> void DrosophilaOmmatidiaJSONProject::WriteFrame(const typename T::Pointer & image, const std::string & name) {
	std::stringstream buffer("");
	buffer << this->m_ProjectPath << "/" << name;
	typedef itk::ImageFileWriter<T> WriterType;

	typename WriterType::Pointer writer = WriterType::New();

	typedef itk::MetaImageIO ImageIOType;
	typename ImageIOType::Pointer imageIO = ImageIOType::New();
	writer->SetImageIO(imageIO);
	std::cout << buffer.str() << std::endl;
	writer->SetFileName(buffer.str());
	writer->SetInput(image);
	writer->Update();
}

bool DrosophilaOmmatidiaJSONProject::Open(const std::string & projectPath) {
	this->m_ProjectPath = projectPath;

	std::stringstream projectFileStream("");
	projectFileStream << m_ProjectPath << "/" << "project.json";
	std::ifstream projectFile(projectFileStream.str(), std::ifstream::in);
	if (!projectFile.is_open()) {
		return false;
	}

	Json::Reader reader;
	Json::Value root;
	reader.parse(projectFile, root);

	this->m_FirstFrame = root["firstFrame"].asInt();
	this->m_LastFrame = root["lastFrame"].asInt();
	this->m_NumFrames = m_LastFrame - m_FirstFrame + 1;

	return true;
}

DrosophilaOmmatidiaJSONProject::OriginalImageType::Pointer DrosophilaOmmatidiaJSONProject::GetOriginalImage(int frame) {

	OriginalImageType::Pointer originalImage { };

	std::stringstream buffer("");
	buffer << "original_L" << m_Level << "_T" << m_FirstFrame + frame << ".mha";

	std::string fileName = buffer.str();
	this->ReadFrame<OriginalImageType>(originalImage, fileName);
	m_Spacing = originalImage->GetSpacing();
	return originalImage;
}

DrosophilaOmmatidiaJSONProject::PlatenessImageType::Pointer DrosophilaOmmatidiaJSONProject::GetPlatenessImage(int frame) {

	PlatenessImageType::Pointer platenessImage { };

	std::stringstream buffer("");
	buffer << "plateness_T" << m_FirstFrame + frame << ".mha";

	std::string fileName = buffer.str();
	this->ReadFrame<PlatenessImageType>(platenessImage, fileName);
	platenessImage->SetSpacing(m_Spacing);
	return platenessImage;
}

DrosophilaOmmatidiaJSONProject::DeconvolutedImageType::Pointer DrosophilaOmmatidiaJSONProject::GetDeconvolutedImage(int frame) {

	DeconvolutedImageType::Pointer deconvolutedImage { };

	std::stringstream buffer("");
	buffer << "estimate_L" << m_Level << "_T" << m_FirstFrame + frame << ".mha";

	std::string fileName = buffer.str();
	this->ReadFrame<DeconvolutedImageType>(deconvolutedImage, fileName);
	return deconvolutedImage;
}

DrosophilaOmmatidiaJSONProject::MotionImageType::Pointer DrosophilaOmmatidiaJSONProject::GetMotionImage(int frame) {

	MotionImageType::Pointer motionImage { };

	std::stringstream buffer("");
	buffer << "registration_L" << m_Level << "_FT" << m_FirstFrame + frame << "_MT" << m_FirstFrame + frame + 1 << ".mha";

	std::string fileName = buffer.str();
	this->ReadFrame<MotionImageType>(motionImage, fileName);
	return motionImage;
}
#if 0
typename DrosophilaOmmatidiaJSONProject::VertexMolecularFeatureMapType::Pointer DrosophilaOmmatidiaJSONProject::GetVertexMolecularDistribution(int frame) {

}

typename DrosophilaOmmatidiaJSONProject::EdgeMolecularFeatureMapType::Pointer DrosophilaOmmatidiaJSONProject::GetEdgeMolecularDistribution(int frame,const typename DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & ajGraph) {

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
		DescriptorType distribution(length);

		for(int l=0;l<length;l++) {
			distribution[l]=root["Molecules"][i]["descriptor"][l].asDouble();
		}
		unsigned long edgeNumber=ajGraph->GetAJEdgeNumber(source,target);
		std::cout << edgeNumber << std::endl;
		typename FeatureDescriptor< DescriptorType>::Pointer descriptor = FeatureDescriptor< DescriptorType>::New();
		descriptor->SetValue(distribution);
		descriptors->AddFeature(edgeNumber,descriptor);

	}
#if 0
	for(auto it = descriptors.begin();it!=descriptors.end();it++) {
		//std::cout << it->first << " " << it->second << std::endl;
	}
#endif
	return descriptors;

}

void DrosophilaOmmatidiaJSONProject::SetVertexMolecularDistribution(int frame, const VertexMolecularFeatureMapType::Pointer & vertexMolecularDistribution) {

	Json::StyledWriter writer;
	Json::Value root;
	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "verticesMolecules" << "_T" << m_FirstFrame+frame << ".json";

	std::string vertexStorageFile;
	fileNameStream >> vertexStorageFile;

	int i=0;

	for(auto it = vertexMolecularDistribution->FeaturesBegin();it!=vertexMolecularDistribution->FeaturesEnd();it++) {
		AdherensJunctionGraphType::AJVertexHandler vertex= it->first;
		typedef itk::Array<double> DistributionType;
		typename FeatureDescriptor< DistributionType>::Pointer descriptor = it->second;
		root["Molecules"][i]["vertex"]=static_cast<Json::UInt64>(vertex);
		root["Molecules"][i]["length"]=static_cast<Json::UInt64>(descriptor->GetValue().size());
		for(int l=0;l<descriptor->GetValue().size();l++) {
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

void DrosophilaOmmatidiaJSONProject::SetEdgeMolecularDistribution(int frame, const EdgeMolecularFeatureMapType::Pointer & edgeMolecularDistribution, const DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & graph) {

	Json::StyledWriter writer;
	Json::Value root;
	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "edgesMolecules" << "_T" << m_FirstFrame+frame << ".json";

	std::string vertexStorageFile;
	fileNameStream >> vertexStorageFile;

	int i=0;

	for(auto it = edgeMolecularDistribution->FeaturesBegin();it!=edgeMolecularDistribution->FeaturesEnd();it++) {

		AdherensJunctionGraphType::AJEdgeHandler edge= graph->GetAJEdgeHandelerFromNumber(it->first);
		typedef itk::Array<double> DistributionType;
		typename FeatureDescriptor< DistributionType>::Pointer descriptor = it->second;

		root["Molecules"][i]["source"]=(Json::UInt64)graph->GetAJEdgeSource(edge);
		root["Molecules"][i]["target"]=(Json::UInt64)graph->GetAJEdgeTarget(edge);
		root["Molecules"][i]["length"]=(Json::UInt64)descriptor->GetValue().size();
		for(int l=0;l<descriptor->GetValue().size();l++) {
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

bool DrosophilaOmmatidiaJSONProject::IsMolecularImage(int frame) {
	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "actin" << "_T" << m_FirstFrame + frame << ".mha";

	std::ifstream file(fileNameStream.str());
	return file.is_open();
}

bool DrosophilaOmmatidiaJSONProject::IsVertexnessImage(int frame) {
	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "vertexness" << "_T" << m_FirstFrame + frame << ".mha";

	std::ifstream file(fileNameStream.str());
	return file.is_open();
}
DrosophilaOmmatidiaJSONProject::MolecularImageType::Pointer DrosophilaOmmatidiaJSONProject::GetMolecularImage(int frame) {

	MolecularImageType::Pointer molecularImage { };

	std::stringstream buffer("");
	buffer << "actin_T" << m_FirstFrame + frame << ".mha";

	std::string fileName = buffer.str();
	this->ReadFrame<MolecularImageType>(molecularImage, fileName);
	return molecularImage;
}

DrosophilaOmmatidiaJSONProject::HessianImageType::Pointer DrosophilaOmmatidiaJSONProject::GetHessianImage(int frame) {
	HessianImageType::Pointer hessianImage { };
	std::stringstream buffer("");
	buffer << "hessian_L" << m_Level << "_T" << m_FirstFrame + frame << ".mha";
	std::string fileName = buffer.str();
	this->ReadFrame<HessianImageType>(hessianImage, fileName);
	hessianImage->SetSpacing(m_Spacing);
	return hessianImage;
}

DrosophilaOmmatidiaJSONProject::VertexnessImageType::Pointer DrosophilaOmmatidiaJSONProject::GetVertexnessImage(int frame) {

	VertexnessImageType::Pointer vertexnessImage { };

	std::stringstream buffer("");
	buffer << "vertexness_T" << m_FirstFrame + frame << ".mha";

	std::string fileName = buffer.str();
	this->ReadFrame<VertexnessImageType>(vertexnessImage, fileName);
	return vertexnessImage;
}

void DrosophilaOmmatidiaJSONProject::SetVertexnessImage(int frame, VertexnessImageType::Pointer & vertexnessImage) {

	std::stringstream buffer("");
	buffer << "vertexness_T" << m_FirstFrame + frame << ".mha";
	std::string fileName = buffer.str();
	this->WriteFrame<VertexnessImageType>(vertexnessImage, fileName);

}

bool DrosophilaOmmatidiaJSONProject::IsAJVertices(int frame) {
	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "vertices" << "_T" << m_FirstFrame + frame << ".json";

	std::ifstream file(fileNameStream.str());
	return file.is_open();

}


void DrosophilaOmmatidiaJSONProject::StoreAJGraph(const std::string & fileName, const DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & ajVertices) {
	Json::StyledWriter writer;
	Json::Value root;

	std::string vertexStorageFile = fileName;

	int i = 0;
	for (auto it = ajVertices->VerticesBegin(); it != ajVertices->VerticesEnd(); it++) {

		AdherensJunctionGraphType::AJVertexType::PointType position = ajVertices->GetAJVertex((*it))->GetPosition();
		auto sphericalEmbeddedPosition = ajVertices->GetAJVertex((*it))->GetEmbeddedSphericalPosition();
		auto embeddedPosition = ajVertices->GetAJVertex((*it))->GetEmbeddedPosition();
		AdherensJunctionGraphType::AJVertexType::VectorType velocity = ajVertices->GetAJVertex((*it))->GetVelocity();

		root["Vertices"][i]["x"] = position[0];
		root["Vertices"][i]["y"] = position[1];
		root["Vertices"][i]["z"] = position[2];

		root["Vertices"][i]["Embedded"][0] = embeddedPosition[0];
		root["Vertices"][i]["Embedded"][1] = embeddedPosition[1];
		root["Vertices"][i]["Embedded"][2] = embeddedPosition[2];

		root["Vertices"][i]["SphericalEmbedded"][0] = sphericalEmbeddedPosition[0];
		root["Vertices"][i]["SphericalEmbedded"][1] = sphericalEmbeddedPosition[1];
		root["Vertices"][i]["SphericalEmbedded"][2] = sphericalEmbeddedPosition[2];

		root["Vertices"][i]["z"] = position[2];

		root["Vertices"][i]["dx"] = velocity[0];
		root["Vertices"][i]["dy"] = velocity[1];
		root["Vertices"][i]["dz"] = velocity[2];

		root["Vertices"][i]["dx"] = velocity[0];
		root["Vertices"][i]["dy"] = velocity[1];
		root["Vertices"][i]["dz"] = velocity[2];

		auto vertex = ajVertices->GetAJVertex((*it));

		int k=0;
		for(auto it2=vertex->BeginCells();it2!=vertex->EndCells();++it2){
			root["Vertices"][i]["c"][k]=(Json::UInt64)*it2;
			k++;
		}
#if 0
				for(auto it2=vertex->)

#endif
		itk::Array<double> descriptor = vertex->GetDescriptor();
		for (int l = 0; l < descriptor.size(); l++) {
			root["Vertices"][i]["descriptor"][l] = descriptor[l];
		}
		i++;
	}
	i = 0;
	for (auto it = ajVertices->EdgesBegin(); it != ajVertices->EdgesEnd(); it++) {
		AdherensJunctionGraphType::AJVertexHandler source, target;
		source = ajVertices->GetAJEdgeSource(*it);
		target = ajVertices->GetAJEdgeTarget(*it);

		root["Edges"][i]["s"] = (Json::UInt64) source;
		root["Edges"][i]["t"] = (Json::UInt64) target;
		root["Edges"][i]["c"][0] = (Json::UInt64)ajVertices->GetAJEdge(*it)->GetCellPair().first;
		root["Edges"][i]["c"][1] = (Json::UInt64)ajVertices->GetAJEdge(*it)->GetCellPair().second;
		itk::Array<double> descriptor = ajVertices->GetAJEdge((*it))->GetDescriptor();
		for (int l = 0; l < descriptor.size(); l++) {
			root["Edges"][i]["descriptor"][l] = descriptor[l];
		}

		i++;
	}

	std::string jsoncontent = writer.write(root);
	std::ofstream file(vertexStorageFile.c_str(), std::ofstream::out | std::ofstream::trunc);

	file << jsoncontent;

	file.close();
}


void DrosophilaOmmatidiaJSONProject::SetAJGraph(int frame, const DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer & ajVertices) {

	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "vertices" << "_T" << m_FirstFrame + frame << ".json";

	this->StoreAJGraph(fileNameStream.str(),ajVertices);

}

DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer DrosophilaOmmatidiaJSONProject::LoadAJGraph(const std::string & fileName) {

	Json::Reader reader;
	Json::Value root;

	std::ifstream jsonAJVerticesFile(fileName.c_str());
	reader.parse(jsonAJVerticesFile, root);

	AdherensJunctionGraphType::Pointer ajGraph = AdherensJunctionGraphType::New();

	int numvertices = root["Vertices"].size();

	for (int i = 0; i < numvertices; i++) {
		AdherensJunctionGraphType::AJVertexType::Pointer newVertex = AdherensJunctionGraphType::AJVertexType::New();

		AdherensJunctionGraphType::AJVertexType::PointType position;

		position[0] = root["Vertices"][i]["x"].asDouble();
		position[1] = root["Vertices"][i]["y"].asDouble();
		position[2] = root["Vertices"][i]["z"].asDouble();
		newVertex->SetPosition(position);

		AdherensJunctionGraphType::AJVertexType::PointType embeddedPosition;
		if(root["Vertices"][i].isMember("Embedded")){
			embeddedPosition[0] = root["Vertices"][i]["Embedded"][0].asDouble() ;
			embeddedPosition[1] = root["Vertices"][i]["Embedded"][1].asDouble() ;
			embeddedPosition[2] = root["Vertices"][i]["Embedded"][2].asDouble() ;

		}
		newVertex->SetEmbeddedPosition(embeddedPosition);
		AdherensJunctionGraphType::AJVertexType::SphericalPointType embeddedSphericalPosition;
		if(root["Vertices"][i].isMember("SphericalEmbedded")){
			embeddedSphericalPosition[0] = root["Vertices"][i]["SphericalEmbedded"][0].asDouble() ;
			embeddedSphericalPosition[1] = root["Vertices"][i]["SphericalEmbedded"][1].asDouble() ;
			embeddedSphericalPosition[2] = root["Vertices"][i]["SphericalEmbedded"][2].asDouble() ;

		}
		newVertex->SetEmbeddedSphericalPosition(embeddedSphericalPosition);




		AdherensJunctionGraphType::AJVertexType::VectorType velocity;
		if (root["Vertices"][i].isMember("dx")) {
			velocity[0] = root["Vertices"][i]["dx"].asDouble();
			velocity[1] = root["Vertices"][i]["dy"].asDouble();
			velocity[2] = root["Vertices"][i]["dz"].asDouble();
			newVertex->SetVelocity(velocity);
		}
		if (root["Vertices"][i].isMember("c")) {
			for (int l = 0; l < root["Vertices"][i]["c"].size(); l++) {
				newVertex->AddCell(root["Vertices"][i]["c"][l].asUInt64());
			}

		}
		unsigned int length = root["Vertices"][i]["descriptor"].size();
		typedef itk::Array<double> DescriptorType;
		DescriptorType distribution(length);

		for (int l = 0; l < length; l++) {
			distribution[l] = root["Vertices"][i]["descriptor"][l].asDouble();
		}

		newVertex->SetDescriptor(distribution);

		ajGraph->AddAJVertex(newVertex);
	}

	int numedges = root["Edges"].size();

	for (int i = 0; i < numedges; i++) {
		AdherensJunctionGraphType::AJVertexHandler source, target;

		source = root["Edges"][i]["s"].asUInt64();
		target = root["Edges"][i]["t"].asUInt64();

		unsigned int length = root["Edges"][i]["descriptor"].size();
		typedef itk::Array<double> DescriptorType;
		DescriptorType distribution(length);
		for (int l = 0; l < length; l++) {
			distribution[l] = root["Edges"][i]["descriptor"][l].asDouble();
		}
		auto edgeHandler = ajGraph->AddAJEdge(source, target);

		//std::cout << edgeHandler << " " << 		boost::get(boost::edge_index,ajGraph->GetBoostGraph(),edgeHandler) <<  std::endl;
		auto edge = ajGraph->GetAJEdge(edgeHandler);
		edge->SetDescriptor(distribution);

		if (root["Edges"][i].isMember("c")) {
			edge->SetCellPair(std::pair<unsigned long, unsigned long>(root["Edges"][i]["c"][0].asUInt64(),root["Edges"][i]["c"][1].asUInt64()));
		}
	}

	return ajGraph;
}

DrosophilaOmmatidiaJSONProject::AdherensJunctionGraphType::Pointer DrosophilaOmmatidiaJSONProject::GetAJGraph(int frame) {


	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "vertices" << "_T" << m_FirstFrame + frame << ".json";

	std::string ajVerticesFile = fileNameStream.str();

	return this->LoadAJGraph(ajVerticesFile);

}
bool DrosophilaOmmatidiaJSONProject::IsTissueDescriptor(int frame) {
	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "tissue" << "_T" << m_FirstFrame + frame << ".json";

	std::ifstream file(fileNameStream.str());
	return file.is_open();

}
void DrosophilaOmmatidiaJSONProject::SetTissueDescriptor(int frame, const DrosophilaOmmatidiaJSONProject::TissueType::Pointer & tissue) {

	std::stringstream embeddedAJGraphfileNameStream;
	embeddedAJGraphfileNameStream << this->m_ProjectPath << "/" << "embeddedVertices" << "_T" << m_FirstFrame + frame << ".json";


	this->StoreAJGraph(embeddedAJGraphfileNameStream.str(),tissue->GetAJGraph());
	Json::StyledWriter writer;
	Json::Value root;
	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "tissue" << "_T" << m_FirstFrame + frame << ".json";

	std::string cellsStorageFile;
	cellsStorageFile = fileNameStream.str();

	int i = 0;
	root["UnboundedFace"] =(Json::UInt64) tissue->GetCellGraph()->GetUnboundedFace();
	for (auto it = tissue->GetCellGraph()->CellsBegin(); it != tissue->GetCellGraph()->CellsEnd(); it++) {
		DrosophilaOmmatidiaJSONProject::CellGraphType::CellType::Pointer cell = tissue->GetCellGraph()->GetCell(*it);
		AdherensJunctionGraphType::AJVertexType::PointType position = cell->GetPosition();
//          AdherensJunctionGraphType::AJVertexType::VectorType velocity = cell->GetVelocity();


		root["Cells"][i]["x"] = position[0];
		root["Cells"][i]["y"] = position[1];
		root["Cells"][i]["z"] = position[2];
		root["Cells"][i]["area"] = cell->GetArea();
		root["Cells"][i]["xx"] = cell->GetXX();
		root["Cells"][i]["xy"] = cell->GetXY();
		root["Cells"][i]["yy"] = cell->GetYY();
		root["Cells"][i]["perimeterLength"] = cell->GetPerimeterLength();
#if 0
		root["Cells"][i]["dx"]=velocity[0];
		root["Cells"][i]["dy"]=velocity[1];
		root["Cells"][i]["dz"]=velocity[2];
#endif
		int k = 0;
		for (auto perimIt = cell->BeginPerimeterAJVertices(); perimIt != cell->EndPerimeterAJVertices(); ++perimIt) {
			root["Cells"][i]["PerimeterVertices"][k] = (Json::UInt64) *perimIt;
			k++;
		}

		k=0;
		for (auto perimIt = cell->BeginPerimeterAJEdges(); perimIt != cell->EndPerimeterAJEdges();++perimIt){
			root["Cells"][i]["PerimeterEdges"][k][0] = (Json::UInt64)tissue->GetAJGraph()->GetAJEdgeSource(*perimIt);
			root["Cells"][i]["PerimeterEdges"][k][1] = (Json::UInt64)tissue->GetAJGraph()->GetAJEdgeTarget(*perimIt);
			k++;
		}
		root["Cells"][i]["type"] = cell->GetCellType();
		i++;
	}
	i = 0;
	for (auto it = tissue->GetCellGraph()->CellEdgesBegin(); it != tissue->GetCellGraph()->CellEdgesEnd(); it++) {
		AdherensJunctionGraphType::AJVertexHandler source, target;
		source = tissue->GetCellGraph()->GetCellEdgeSource(*it);
		target = tissue->GetCellGraph()->GetCellEdgeTarget(*it);

		root["Edges"][i]["s"] = (Json::UInt64) source;
		root["Edges"][i]["t"] = (Json::UInt64) target;
#if 0
		itk::Array<double > descriptor=ajVertices->GetAJEdge((*it))->GetDescriptor();
		for(int l=0;l<descriptor.size();l++) {
			root["Edges"][i]["descriptor"][l]=descriptor[l];
		}
#endif
		i++;
	}

	std::string jsoncontent = writer.write(root);
	std::ofstream file(cellsStorageFile.c_str(), std::ofstream::out | std::ofstream::trunc);

	file << jsoncontent;

	file.close();
}

DrosophilaOmmatidiaJSONProject::TissueType::Pointer DrosophilaOmmatidiaJSONProject::GetTissueDescriptor(int frame) {

	TissueType::Pointer tissue = TissueType::New();


	std::stringstream embeddedAJGraphfileNameStream;
	embeddedAJGraphfileNameStream << this->m_ProjectPath << "/" << "embeddedVertices" << "_T" << m_FirstFrame + frame << ".json";


	tissue->SetAJGraph(this->LoadAJGraph(embeddedAJGraphfileNameStream.str()));


	auto cellGraph = TissueType::CellGraphType::New();
	tissue->SetCellGraph(cellGraph);


	Json::Reader reader;
	Json::Value root;

	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "tissue_T" << m_FirstFrame + frame << ".json";

	std::string cellsFile;
	cellsFile = fileNameStream.str();

	std::cout << cellsFile << std::endl;

	std::ifstream jsonCellsFile(cellsFile.c_str());
	reader.parse(jsonCellsFile, root);


	cellGraph->SetUnboundedFace(root["UnboundedFace"].asUInt64());
	int numvertices = root["Cells"].size();

	for (int i = 0; i < numvertices; i++) {
		CellGraphType::CellType::Pointer newCell = CellGraphType::CellType::New();

		AdherensJunctionGraphType::AJVertexType::PointType position;

		position[0] = root["Cells"][i]["x"].asDouble();
		position[1] = root["Cells"][i]["y"].asDouble();
		position[2] = root["Cells"][i]["z"].asDouble();
		newCell->SetPosition(position);

		newCell->SetArea(root["Cells"][i]["area"].asDouble());
		newCell->SetXX(root["Cells"][i]["xx"].asDouble());
		newCell->SetXY(root["Cells"][i]["xy"].asDouble());
		newCell->SetYY(root["Cells"][i]["yy"].asDouble());
		newCell->SetPerimeterLength(root["Cells"][i]["perimeterLength"].asDouble());

		int perimeterVerticesLength = root["Cells"][i]["PerimeterVertices"].size();

		for (int k = 0; k < perimeterVerticesLength; k++) {
			newCell->AddAJVertexToPerimeter(root["Cells"][i]["PerimeterVertices"][k].asUInt64());
		}

		int perimeterEdgesLength = root["Cells"][i]["PerimeterVertices"].size();

		for (int k = 0; k < perimeterVerticesLength; k++) {
			newCell->AddAJEdgeToPerimeter(tissue->GetAJGraph()->GetAJEdgeHandler(root["Cells"][i]["PerimeterEdges"][k][0].asUInt64(),root["Cells"][i]["PerimeterEdges"][k][1].asUInt64()));
		}
		if (root["Cells"][i].isMember("type")) {
			newCell->SetCellType(root["Cells"][i]["type"].asInt());
		}
		cellGraph->AddCell(newCell);
	}

	int numedges = root["Edges"].size();

	for (int i = 0; i < numedges; i++) {
		AdherensJunctionGraphType::AJVertexHandler source, target;

		source = root["Edges"][i]["s"].asUInt64();
		target = root["Edges"][i]["t"].asUInt64();
		auto edgeHandler = cellGraph->AddCellEdge(source, target);

	}


	return tissue;
}




typename DrosophilaOmmatidiaJSONProject::CorrespondenceSetType DrosophilaOmmatidiaJSONProject::GetCorrespondences(int frame0, int frame1) {
	CorrespondenceSetType correspondences;

	Json::Reader reader;
	Json::Value root;

	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "correspondence_T" << m_FirstFrame + frame0 << "_T" << m_FirstFrame + frame1 << ".json";

	std::string correspondenceFile;
	correspondenceFile = fileNameStream.str();

	std::cout << correspondenceFile << std::endl;

	std::ifstream jsonCorrespondenceFile(correspondenceFile.c_str());
	reader.parse(jsonCorrespondenceFile, root);

	{
		int numCorrespondences = root["Correspondences"].size();
		for (int i = 0; i < numCorrespondences; i++) {
			CorrespondenceSetType::AJSubgraphType antecessor;
			CorrespondenceSetType::AJSubgraphType successor;

			antecessor.SetOrder(root["Correspondence"][i]["antecessorOrder"].asUInt());
			for(int k=0;k < root["Correspondence"][i]["antecessor"].size();k++){
				antecessor.AddVertex(root["Correspondence"][i]["antecessor"][k].asUInt64());

			}

			successor.SetOrder(root["Correspondence"][i]["successorOrder"].asUInt());
			for(int k=0;k < root["Correspondence"][i]["successor"].size();k++){
				successor.AddVertex(root["Correspondence"][i]["successor"][k].asUInt64());
			}
			double cost=root["Correspondence"][i]["cost"].asDouble();
			correspondences.AddCorrespondence(antecessor,successor,cost);
		}
	}
#if 0


	Json::Reader reader;
	Json::Value root;

	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "correspondence_T" << m_FirstFrame + frame0 << "_T" << m_FirstFrame + frame1 << ".json";

	std::string correspondenceFile;
	correspondenceFile = fileNameStream.str();

	std::cout << correspondenceFile << std::endl;

	std::ifstream jsonCorrespondenceFile(correspondenceFile.c_str());
	reader.parse(jsonCorrespondenceFile, root);

	{
	int numSuccesors = root["Succesor"].size();

	for (int i = 0; i < numSuccesors; i++) {

		typename CorrespondenceType::CliqueType succesor0,succesor1;

		succesor0.AddVertex(root["Succesor"][i][0].asUInt64());
		succesor1.AddVertex(root["Succesor"][i][1].asUInt64());

		succesor.SetV1(root["Succesor"][i][1].asUInt64());

		correspondence->AddCorrespondence(succesor);
	}
	}
	{
	int numEnterings = root["Entering"].size();

	for (int i = 0; i < numEnterings; i++) {
		CorrespondenceType::EnteringType entering;

		entering.SetV1(root["Entering"][i].asUInt64());
		correspondence->AddCorrespondence(entering);
	}
	}
	{
	int numLeavings = root["Leaving"].size();
	for (int i = 0; i < numLeavings; i++) {
		CorrespondenceType::LeavingType leaving;

		leaving.SetV0(root["Leaving"][i].asUInt64());
		correspondence->AddCorrespondence(leaving);
	}
	}
	{
	int numMerge = root["Merge"].size();
	for (int i = 0; i < numMerge; i++) {
		CorrespondenceType::MergeType merge;

		merge.SetV0A(root["Merge"][i][0].asUInt64());
		merge.SetV0B(root["Merge"][i][1].asUInt64());
		merge.SetV1(root["Merge"][i][2].asUInt64());
		correspondence->AddCorrespondence(merge);
	}
	}
	{
	int numSplits = root["Split"].size();

	for (int i = 0; i < numSplits; i++) {
		CorrespondenceType::SplitType split;

		split.SetV0(root["Split"][i][0].asUInt64());
		split.SetV1A(root["Split"][i][1].asUInt64());
		split.SetV1B(root["Split"][i][2].asUInt64());

		correspondence->AddCorrespondence(split);
	}
	}
	{
	int numT1s = root["T1"].size();

	for (int i = 0; i < numT1s; i++) {
		CorrespondenceType::T1Type t1;

		t1.SetV0A(root["T1"][i][0].asUInt64());
		t1.SetV0B(root["T1"][i][1].asUInt64());
		t1.SetV1A(root["T1"][i][2].asUInt64());
		t1.SetV1B(root["T1"][i][3].asUInt64());
		correspondence->AddCorrespondence(t1);
	}
	}
	{
	int numT2s = root["T2"].size();
	for (int i = 0; i < numT2s; i++) {
		CorrespondenceType::T2Type t2;

		for(int k=0;k < root["T2"][i]["source"].size();k++){
			t2.AddV0(root["T2"][i]["source"][k].asUInt64());
		}

		for(int k=0;k < root["T2"][i]["target"].size();k++){
			t2.AddV1(root["T2"][i]["target"][k].asUInt64());
		}
		correspondence->AddCorrespondence(t2);
	}
	}

	return correspondence;
#endif
	return correspondences;
}

void DrosophilaOmmatidiaJSONProject::SetCorrespondences(int frame0, int frame1,
		const typename DrosophilaOmmatidiaJSONProject::CorrespondenceSetType & correspondences) {

	Json::Value root;
	Json::StyledWriter writer;

	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "correspondence_T" << m_FirstFrame + frame0 << "_T" << m_FirstFrame + frame1 << ".json";

	std::string correspondenceFile;
	correspondenceFile = fileNameStream.str();

	std::cout << correspondenceFile << std::endl;

	int i = 0;
	for (auto it = correspondences.BeginCorrespondences(); it != correspondences.EndCorrespondences(); it++) {
		int k = 0;
		auto antecessor = it->GetAntecessor();
		root["Correspondence"][i]["antecessorOrder"]=antecessor.GetOrder();
		for (auto it2 =antecessor.BeginVertices(); it2 != antecessor.EndVertices(); it2++) {
			root["Correspondence"][i]["antecessor"][k] = (Json::UInt64) *it2;
			k++;
		}

		k = 0;
		auto successor = it->GetSuccessor();
		root["Correspondence"][i]["successorOrder"]=successor.GetOrder();
		for (auto it2 =successor.BeginVertices(); it2 != successor.EndVertices(); it2++) {
			root["Correspondence"][i]["successor"][k] = (Json::UInt64) *it2;
			k++;
		}
		root["Correspondence"][i]["cost"]=it->GetCost();
		i++;
	}

	std::string jsoncontent = writer.write(root);
	std::ofstream file(correspondenceFile.c_str(), std::ofstream::out | std::ofstream::trunc);

	file << jsoncontent;

	file.close();

}
#if 0
DrosophilaOmmatidiaJSONProject::CellCorrespondenceType::Pointer DrosophilaOmmatidiaJSONProject::GetCellCorrespondences(int frame0, int frame1) {

	typename CellCorrespondenceType::Pointer cellCorrespondence = CellCorrespondenceType::New();

	Json::Reader reader;
	Json::Value root;

	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "cellCorrespondence_T" << m_FirstFrame + frame0 << "_T" << m_FirstFrame + frame1 << ".json";

	std::string correspondenceFile;
	correspondenceFile = fileNameStream.str();

	std::cout << correspondenceFile << std::endl;

	std::ifstream jsonCorrespondenceFile(correspondenceFile.c_str());
	reader.parse(jsonCorrespondenceFile, root);

	{
		int numSuccesors = root["Succesor"].size();

		for (int i = 0; i < numSuccesors; i++) {

			CellCorrespondenceType::CellSuccesorType succesor;

			succesor.SetC0(root["Succesor"][i][0].asUInt64());
			succesor.SetC1(root["Succesor"][i][1].asUInt64());


			cellCorrespondence->AddCorrespondence(succesor);
		}
	}
		{
		int numEnterings = root["Entering"].size();

		for (int i = 0; i < numEnterings; i++) {
			CellCorrespondenceType::CellEnteringType entering;

			entering.SetC1(root["Entering"][i].asUInt64());
			cellCorrespondence->AddCorrespondence(entering);
		}
		}
		{
		int numLeavings = root["Leaving"].size();
		for (int i = 0; i < numLeavings; i++) {
			CellCorrespondenceType::CellLeavingType leaving;

			leaving.SetC0(root["Leaving"][i].asUInt64());
			cellCorrespondence->AddCorrespondence(leaving);
		}
		}
		{
		int numApoptosis = root["Apoptosis"].size();
		for (int i = 0; i < numApoptosis; i++) {
			CellCorrespondenceType::CellApoptosisType apoptosis;

			apoptosis.SetC0(root["Apoptosis"][i].asUInt64());
			cellCorrespondence->AddCorrespondence(apoptosis);
		}
		}
		return cellCorrespondence;
}


void DrosophilaOmmatidiaJSONProject::SetCellCorrespondences(int frame0, int frame1,
		const DrosophilaOmmatidiaJSONProject::CellCorrespondenceType::Pointer & cellCorrespondences) {

	Json::Value root;
	Json::StyledWriter writer;

	std::stringstream fileNameStream;
	fileNameStream << this->m_ProjectPath << "/" << "cellCorrespondence_T" << m_FirstFrame + frame0 << "_T" << m_FirstFrame + frame1 << ".json";

	std::string correspondenceFile;
	correspondenceFile = fileNameStream.str();

	std::cout << correspondenceFile << std::endl;

	int i = 0;
	for (auto it = cellCorrespondences->BeginCellSuccesors(); it != cellCorrespondences->EndCellSuccesors(); it++) {
		auto succesor = *it;
		root["Succesor"][i][0] = (Json::UInt64) succesor.GetC0();
		root["Succesor"][i][1] = (Json::UInt64) succesor.GetC1();
		i++;
	}

	i = 0;
	for (auto it = cellCorrespondences->BeginCellEnterings(); it != cellCorrespondences->EndCellEnterings(); it++) {
		auto entering = *it;
		root["Entering"][i] = (Json::UInt64) entering.GetC1();
		i++;
	}

	i = 0;
	for (auto it = cellCorrespondences->BeginCellLeavings(); it != cellCorrespondences->EndCellLeavings(); it++) {
		auto leaving = *it;
		root["Leaving"][i] = (Json::UInt64) leaving.GetC0();
		i++;
	}

	i = 0;
	for (auto it = cellCorrespondences->BeginCellApoptosis(); it != cellCorrespondences->EndCellApoptosis(); it++) {

		root["Apoptosis"][i][0] = (Json::UInt64) it->GetC0();
		i++;
	}

	std::string jsoncontent = writer.write(root);
	std::ofstream file(correspondenceFile.c_str(), std::ofstream::out | std::ofstream::trunc);

	file << jsoncontent;

	file.close();

}


#endif
