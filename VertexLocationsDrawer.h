//    This file is part of TTT Tissue Tracker.
//
//    TTT Tissue Tracker is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    TTT Tissue Tracker is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with TTT Tissue Tracker.  If not, see <http://www.gnu.org/licenses/>.

/** \addtogroup TTTDrawer
 *  @{
 */
#ifndef VERTEX_LOCATIONS_DRAWER_H
#define VERTEX_LOCATIONS_DRAWER_H

#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>

#include <vtkActor.h>
#include <vtkProperty.h>
#include <itkFixedArray.h>

#include <boost/tuple/tuple.hpp>

#include "Drawer.h"
#include <map>

template<class TVerticesContainer> class VertexLocationsDrawer : public ttt::Drawer {
public:

private:
    typedef itk::FixedArray<float,3> SpacingType;

    typename TVerticesContainer::Pointer m_VertexLocations;

    std::map<vtkSmartPointer<vtkActor>,typename TVerticesContainer::AJVertexHandler> m_ActorsToVertices;
    std::map<typename TVerticesContainer::AJVertexHandler,vtkSmartPointer<vtkActor>> m_VerticesToActors;
    std::map<typename TVerticesContainer::AJVertexHandler,vtkSmartPointer<vtkSphereSource>> m_VerticesToSources;


public:
	static VertexLocationsDrawer* New(){
		return new VertexLocationsDrawer;
	}

    VertexLocationsDrawer(){
        //m_Spacing.Fill(1);
    }

    void Reset(){

        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();++it){
            m_Renderer->RemoveActor(it->first);
    	}
        m_ActorsToVertices.clear();
        m_VerticesToActors.clear();
        m_VerticesToSources.clear();

    }
    inline void SetVertexLocations(const  typename TVerticesContainer::Pointer & vertexLocations){
    	m_VertexLocations=vertexLocations;
    }


    typename TVerticesContainer::AJVertexHandler GetVertexFromActor(const vtkSmartPointer<vtkActor> & actor){

        auto it= m_ActorsToVertices.find(actor);
        assert(it!=m_ActorsToVertices.end());
        return it->second;
   	}

    void HighlightVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){
        vtkSmartPointer<vtkActor> actor=m_VerticesToActors[vertexHandler];
        assert(actor);
        actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    }

    void DeemphasizeAJVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){
        vtkSmartPointer<vtkActor> actor=m_VerticesToActors[vertexHandler];
        assert(actor);
        actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    }

    virtual void Show(){
        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();it++){
            it->first->VisibilityOn();
    	}
    }
    virtual void Hide(){
        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();it++){
            it->first->VisibilityOff();
    	}
    }

    virtual  void PickOn(){
        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();it++){
            it->first->PickableOn();
        }
    }
    virtual  void PickOff(){
        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();it++){
            it->first->PickableOff();
        }
    }

    virtual void UpdateAJVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){
        const typename TVerticesContainer::AJVertexType::Pointer  vertex=this->m_VertexLocations->GetAJVertex(vertexHandler);
        assert(vertex);

        m_VerticesToSources[vertexHandler]->SetCenter(vertex->GetPosition()[0] ,vertex->GetPosition()[1] ,vertex->GetPosition()[2]);
        m_VerticesToSources[vertexHandler]->Update();



    }

    virtual vtkSmartPointer<vtkActor> DrawAJVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){
        const typename TVerticesContainer::AJVertexType::Pointer  vertex=this->m_VertexLocations->GetAJVertex(vertexHandler);

        vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetCenter(vertex->GetPosition()[0] ,vertex->GetPosition()[1] ,vertex->GetPosition()[2]);

        //sphereSource->SetRadius(2 * m_Spacing[0]);
        sphereSource->SetRadius(0.001);

        sphereSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

        sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

        vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();

        sphereActor->SetMapper(sphereMapper);


        m_Renderer->AddActor(sphereActor);
        sphereActor->VisibilityOff();
        sphereActor->PickableOff();
        sphereActor->GetProperty()->SetColor(1.0,0.0,0.0);

        this->m_ActorsToVertices[sphereActor]=vertexHandler;
        this->m_VerticesToActors[vertexHandler]=sphereActor;
        this->m_VerticesToSources[vertexHandler]=sphereSource;
        return sphereActor;
    }


    virtual void Draw(){
        this->Reset();
        for (auto it = m_VertexLocations->VerticesBegin(); it != m_VertexLocations->VerticesEnd(); it++) {
            this->DrawAJVertex(*it);

        }

    }

	virtual ~VertexLocationsDrawer(){

	}
};

#endif
/** @}*/
