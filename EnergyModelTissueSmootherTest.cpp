//#include <gtest/gtest.h>
#include "DrosophilaOmmatidiaJSONProject.h"

#include "tttEnergyModelTissueSmoother.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vnl/vnl_sample.h>
//TEST(MinCostMaxFlowAJAssociationCommand,GeneralTest){
int main(){
	DrosophilaOmmatidiaJSONProject project;
	project.Open("/home/morgan/dataset/eyes/aCat-LifeR Movies/mhas/26aCATLifeRx3/");


	typedef EnergyModelTissueSmoother<typename DrosophilaOmmatidiaJSONProject::TissueType, typename DrosophilaOmmatidiaJSONProject::PlatenessImageType> SmootherType;

	SmootherType smoother ;
	int frame=0;
	auto tissue = project.GetTissueDescriptor(frame);
#if 0
	for(auto itVertices=tissue->GetAJGraph()->VerticesBegin();itVertices!=tissue->GetAJGraph()->VerticesEnd();++itVertices ){
		auto position = tissue->GetAJGraph()->GetAJVertex(*itVertices)->GetPosition();

		position[0]+=vnl_sample_normal(0,0.0003);
		position[1]+=vnl_sample_normal(0,0.0003);
		position[2]+=vnl_sample_normal(0,0.0003);
		tissue->GetAJGraph()->GetAJVertex(*itVertices)->SetPosition(position);
	}
#endif
	smoother.SetInputTissue(tissue);

	auto plateness= project.GetPlatenessImage(frame);
	auto vertexness= project.GetVertexnessImage(frame);
	smoother.SetPlatenessImage(plateness);
	smoother.SetVertexnessImage(vertexness);
	smoother.DoADMM();
};
#if 0
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif
