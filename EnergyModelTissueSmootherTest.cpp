//#include <gtest/gtest.h>



//#include <vtkRenderWindow.h>
//#include <vtkSmartPointer.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkLineSource.h>
//#include <vtkPolyDataMapper.h>
//#include <vtkSphereSource.h>
//#include <vnl/vnl_sample.h>
//TEST(MinCostMaxFlowAJAssociationCommand,GeneralTest){
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include "MinMaxVolumeDrawer.h"
#include "drawPrimal.h"

#include "DrosophilaOmmatidiaJSONProject.h"

#include "tttEnergyModelTissueSmoother.h"
int main(){
	DrosophilaOmmatidiaJSONProject project;
	//project.Open("/home/morgan/dataset/eyes/aCat-LifeR Movies/mhas/26aCATLifeRx3/");
	project.Open("\\\\rstore1\\tusm_lab_hatini_confocal01$\\data4stevepaper\\26aCATLifeRx3\\");


	
	
	for (int frame = 0; frame < project.GetNumberOfFrames(); frame++){
	//for (int frame = 0; frame < 10; frame++){
		typedef EnergyModelTissueSmoother<typename DrosophilaOmmatidiaJSONProject::TissueType, typename DrosophilaOmmatidiaJSONProject::PlatenessImageType> SmootherType;

		SmootherType smoother;
		auto tissue = project.GetTissueDescriptor(frame);
		vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

		//MinMaxVolumeDrawer<typename DrosophilaOmmatidiaJSONProject::PlatenessImageType> platenessDrawer;

		auto plateness = project.GetPlatenessImage(frame);
		auto vertexness = project.GetVertexnessImage(frame);

		//platenessDrawer.SetRenderer(renderer);
		//platenessDrawer.SetImage(plateness);
		//platenessDrawer.SetOpacity(0.05);
		//platenessDrawer.Draw();
		//platenessDrawer.SetVisibility(true);

		//drawPrimal(tissue, renderer);

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

		smoother.SetPlatenessImage(plateness);
		smoother.SetVertexnessImage(vertexness);
		smoother.DoADMM();
		auto tissue1 = smoother.GetOutputTissue();
		project.SetTissueDescriptor(frame, tissue1);
		project.SetAJGraph(frame, tissue1->GetAJGraph());
		//drawPrimal(tissue1, renderer);

	}

	

};
#if 0
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif
