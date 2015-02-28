#include "VertexMolecularDistributionDescriptor.h"
#include "EdgeMolecularDistributionDescriptor.h"

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <AJGraph.h>
#include <AJVertex.h>
#include <itkSCIFIOImageIO.h>
int main(int argc,char ** argv){

    typedef itk::Image<double,3> MolecularImageType;

    typedef itk::ImageFileReader<MolecularImageType> ReaderType;
    typedef itk::ImageFileWriter<MolecularImageType> WriterType;


    typedef AJGraph<AJVertex> AJGraphType;


    typename ReaderType::Pointer reader = ReaderType::New();



    //reader->SetImageIO(io);
    reader->SetFileName(argv[1]);
    reader->Update();

    typename MolecularImageType::Pointer molecularImage = reader->GetOutput();
    molecularImage->DisconnectPipeline();

    typename MolecularImageType::IndexType indexA,indexB,indexC,indexD,indexE,indexF;
    typename MolecularImageType::PointType  pointA,pointB,pointC,pointD,pointE,pointF;

    typename AJGraphType::AJVertexHandler handlerA, handlerB, handlerC,handlerD,handlerE,handlerF;

    indexA[0]=1;
    indexA[1]=1;
    indexA[2]=1;

    indexB[0]=5;
    indexB[1]=1;
    indexB[2]=1;

    indexC[0]=3;
    indexC[1]=3;
    indexC[2]=1;

    indexD[0]=3;
    indexD[1]=6;
    indexD[2]=1;

    indexE[0]=5;
    indexE[1]=8;
    indexE[2]=1;

    indexF[0]=1;
    indexF[1]=8;
    indexF[2]=1;

    molecularImage->TransformIndexToPhysicalPoint(indexA,pointA);
    molecularImage->TransformIndexToPhysicalPoint(indexB,pointB);
    molecularImage->TransformIndexToPhysicalPoint(indexC,pointC);
    molecularImage->TransformIndexToPhysicalPoint(indexD,pointD);
    molecularImage->TransformIndexToPhysicalPoint(indexE,pointE);
    molecularImage->TransformIndexToPhysicalPoint(indexF,pointF);


    typename AJGraphType::Pointer ajGraph=AJGraphType::New();


    typename AJGraphType::AJVertexType::Pointer vertexA= AJGraphType::AJVertexType::New();
    vertexA->SetPosition(pointA);
    handlerA=ajGraph->AddAJVertex(vertexA);

    typename AJGraphType::AJVertexType::Pointer vertexB=  AJGraphType::AJVertexType::New();
    vertexB->SetPosition(pointB);
    handlerB=ajGraph->AddAJVertex(vertexB);

    typename AJGraphType::AJVertexType::Pointer vertexC=  AJGraphType::AJVertexType::New();
    vertexC->SetPosition(pointC);
    handlerC=ajGraph->AddAJVertex(vertexC);

    typename AJGraphType::AJVertexType::Pointer vertexD=  AJGraphType::AJVertexType::New();
    vertexD->SetPosition(pointD);
    handlerD=ajGraph->AddAJVertex(vertexD);

    typename AJGraphType::AJVertexType::Pointer vertexE=  AJGraphType::AJVertexType::New();
    vertexE->SetPosition(pointE);
    handlerE=ajGraph->AddAJVertex(vertexE);

    typename AJGraphType::AJVertexType::Pointer vertexF=  AJGraphType::AJVertexType::New();
    vertexF->SetPosition(pointF);
    handlerF=ajGraph->AddAJVertex(vertexF);

    ajGraph->AddAJEdge(handlerA,handlerC);
    ajGraph->AddAJEdge(handlerB,handlerC);
    ajGraph->AddAJEdge(handlerC,handlerD);
    ajGraph->AddAJEdge(handlerD,handlerE);
    ajGraph->AddAJEdge(handlerD,handlerF);

    typedef VertexMolecularDistributionDescriptor<AJGraphType,MolecularImageType> VertexMolecularDistributionDescriptorType;

    typename VertexMolecularDistributionDescriptorType::Pointer descriptor =VertexMolecularDistributionDescriptorType::New();

    descriptor->SetRadius(molecularImage->GetSpacing()[0]*2);

    descriptor->SetAJGraph(ajGraph);
    descriptor->SetMolecularImage(molecularImage);

    descriptor->Compute();


    typedef EdgeMolecularDistributionDescriptor<AJGraphType,MolecularImageType> EdgeMolecularDistributionDescriptorType;

    typename EdgeMolecularDistributionDescriptorType::Pointer edgeDescriptor =EdgeMolecularDistributionDescriptorType::New();

    edgeDescriptor->SetRadius(molecularImage->GetSpacing()[0]*2);

    edgeDescriptor->SetAJGraph(ajGraph);
    edgeDescriptor->SetMolecularImage(molecularImage);
    edgeDescriptor->SetNumberOfSegments(3);
    edgeDescriptor->Compute();

}
