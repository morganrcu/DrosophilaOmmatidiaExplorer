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
#include <vtkColor.h>
#include <map>
#include <boost/bimap.hpp>

template<class TVerticesContainer> class VertexLocationsDrawer : public ttt::Drawer {
public:

private:
    typedef itk::FixedArray<float,3> SpacingType;

    typename TVerticesContainer::Pointer m_VertexLocations;

    typedef boost::bimap< boost::bimaps::set_of<vtkSmartPointer<vtkActor> >, boost::bimaps::set_of<typename TVerticesContainer::AJVertexHandler > > ActorVertexContainer;
    ActorVertexContainer m_ActorsToVertices;
    typedef typename ActorVertexContainer::value_type ActorVertexPair;



    std::map<typename TVerticesContainer::AJVertexHandler,vtkSmartPointer<vtkSphereSource> > m_VerticesToSources;


public:
	static VertexLocationsDrawer* New(){
		return new VertexLocationsDrawer;
	}


    VertexLocationsDrawer(){
        //m_Spacing.Fill(1);
    }

    void Reset(){

        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();++it){
            m_Renderer->RemoveActor(it->left);
    	}
        m_ActorsToVertices.clear();
        m_VerticesToSources.clear();

    }
    inline void SetVertexLocations(const  typename TVerticesContainer::Pointer & vertexLocations){
    	m_VertexLocations=vertexLocations;
    }


    typename TVerticesContainer::AJVertexHandler GetVertexFromActor(const vtkSmartPointer<vtkActor> & actor){

        return m_ActorsToVertices.left.at(actor);

   	}

    void HighlightAJVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){
        vtkSmartPointer<vtkActor> actor=m_ActorsToVertices.right.at(vertexHandler);
        assert(actor);
        actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    }

    void DeemphasizeAJVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){
        vtkSmartPointer<vtkActor> actor=m_ActorsToVertices.right.at(vertexHandler);
        assert(actor);
        actor->GetProperty()->SetColor(1.0, 1.0, 0.0);
    }

    virtual void Show(){
        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();it++){
            it->left->VisibilityOn();
    	}
    }
    virtual void Hide(){
        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();it++){
            it->left->VisibilityOff();
    	}
    }

    virtual  void PickOn(){
        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();it++){
            it->left->PickableOn();
        }
    }
    virtual  void PickOff(){
        for(auto it= m_ActorsToVertices.begin();it!=m_ActorsToVertices.end();it++){
            it->left->PickableOff();
        }
    }

    virtual void UpdateAJVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){
        const typename TVerticesContainer::AJVertexType::Pointer  vertex=this->m_VertexLocations->GetAJVertex(vertexHandler);
        assert(vertex);

        m_VerticesToSources[vertexHandler]->SetCenter(vertex->GetPosition()[0] ,vertex->GetPosition()[1] ,vertex->GetPosition()[2]);
        m_VerticesToSources[vertexHandler]->Update();



    }
    virtual void SetVertexColor(const typename TVerticesContainer::AJVertexHandler & vertexHandler,const vtkColor3d & color){
    	this->m_ActorsToVertices.right.at(vertexHandler)->GetProperty()->SetColor(color[0],color[1],color[2]);
    }

    virtual vtkSmartPointer<vtkActor> DrawAJVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){
        const typename TVerticesContainer::AJVertexType::Pointer  vertex=this->m_VertexLocations->GetAJVertex(vertexHandler);

        vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetCenter(vertex->GetPosition()[0] ,vertex->GetPosition()[1] ,vertex->GetPosition()[2]);

        //sphereSource->SetRadius(2 * m_Spacing[0]);
        sphereSource->SetRadius(0.0005);

        sphereSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

        sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

        vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();

        sphereActor->SetMapper(sphereMapper);


        m_Renderer->AddActor(sphereActor);
        sphereActor->VisibilityOff();
        sphereActor->PickableOff();
#if 0
		vtkSmartPointer<vtkLookupTable> colorMap = vtkSmartPointer<vtkLookupTable>::New(); // hot color map

		colorMap->SetRange( 0.0, m_VertexLocations->GetNumVertices() );
		//colorMap->SetHueRange( 0.0, 0.1 );
		//colorMap->SetValueRange( 0.4, 0.8 );
		colorMap->Build();
		double color[3];
		colorMap->GetColor(vertexHandler,color);
        sphereActor->GetProperty()->SetColor(color[0],color[1],color[2]);
#endif
#if 1
        int degree=this->m_VertexLocations->AJVertexDegree(vertexHandler);
        if(degree==1){
        	sphereActor->GetProperty()->SetColor(1.0,0.0,0.0);
        }else if(degree==2){
        	sphereActor->GetProperty()->SetColor(0.0,1.0,0.0);
        }else if(degree==3){
        	sphereActor->GetProperty()->SetColor(0.0,0.0,1.0);
        }
#endif
    //	sphereActor->GetProperty()->SetColor(0.0,0.0,0.0);
    	sphereActor->GetProperty()->SetSpecular(1.0);
        this->m_ActorsToVertices.insert(ActorVertexPair(sphereActor,vertexHandler));

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
