/*
 * drawPrimal.h
 *
 *  Created on: Oct 30, 2015
 *      Author: morgan
 */

#ifndef DRAWPRIMAL_H_
#define DRAWPRIMAL_H_
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <OmmatidiaTissue.h>
void drawPrimal(const OmmatidiaTissue<3>::Pointer & descriptor,const vtkSmartPointer<vtkRenderer> & renderer){
#if 1
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();

	renWin->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);
	double color[3];
	color[0]=double(std::rand())/RAND_MAX;
	color[1]=double(std::rand())/RAND_MAX;
	color[2]=double(std::rand())/RAND_MAX;

	for(auto itVertices = descriptor->GetAJGraph()->VerticesBegin();
			itVertices!=descriptor->GetAJGraph()->VerticesEnd();
			++itVertices){
		auto  vertex=descriptor->GetAJGraph()->GetAJVertex(*itVertices);

        vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();

        sphereSource->SetCenter(vertex->GetPosition()[0],vertex->GetPosition()[1],vertex->GetPosition()[2]);
        sphereSource->SetRadius(0.00001);
        sphereSource->Update();


        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

        mapper->SetInputData(sphereSource->GetOutput());


        vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();

        sphereActor->SetMapper(mapper);
        sphereActor->VisibilityOn();
        sphereActor->SetPickable(false);
        sphereActor->GetProperty()->SetColor(color[0], color[1], color[2]);
        renderer->AddActor(sphereActor);

	}
	for(auto itEdges = descriptor->GetAJGraph()->EdgesBegin();
			itEdges!=descriptor->GetAJGraph()->EdgesEnd();
			++itEdges){


        auto source =descriptor->GetAJGraph()->GetAJEdgeSource(*itEdges);
        auto target =descriptor->GetAJGraph()->GetAJEdgeTarget(*itEdges);


        auto  sourceVertex=descriptor->GetAJGraph()->GetAJVertex(source);
        auto  targetVertex=descriptor->GetAJGraph()->GetAJVertex(target);

        vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();

        lineSource->SetPoint1(sourceVertex->GetPosition()[0],sourceVertex->GetPosition()[1],sourceVertex->GetPosition()[2]);
        lineSource->SetPoint2(targetVertex->GetPosition()[0],targetVertex->GetPosition()[1],targetVertex->GetPosition()[2]);
        lineSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

        mapper->SetInputData(lineSource->GetOutput());


        vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();

        lineActor->SetMapper(mapper);
        lineActor->VisibilityOn();
        lineActor->SetPickable(false);
        lineActor->GetProperty()->SetColor(color[0], color[1], color[2]);
        //lineActor->GetProperty()->SetColor(color[0],color[1],color[2]);

        renderer->AddActor(lineActor);

	}

	renWin->Render();
	iren->Start();
#endif
};
#endif /* DRAWPRIMAL_H_ */
