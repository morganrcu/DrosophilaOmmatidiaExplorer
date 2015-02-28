#ifndef EDGESDRAWER_H
#define EDGESDRAWER_H

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


#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>

#include <vtkActor.h>
#include <vtkProperty.h>
#include <itkFixedArray.h>

#include <boost/tuple/tuple.hpp>

#include "Drawer.h"
#include <map>

template<class TEdgesContainer> class EdgesDrawer : public ttt::Drawer {
public:

private:

    typename TEdgesContainer::Pointer m_EdgesContainer;


    std::map<vtkSmartPointer<vtkActor>,typename TEdgesContainer::AJEdgeHandler> m_ActorsToEdges;
    std::map<typename TEdgesContainer::AJEdgeHandler,vtkSmartPointer<vtkActor>> m_EdgesToActors;


public:
    EdgesDrawer(){

    }

    void Reset(){

        for(auto it= m_ActorsToEdges.begin();it!=m_ActorsToEdges.end();++it){
            m_Renderer->RemoveActor(it->first);
        }

        m_ActorsToEdges.clear();
        m_EdgesToActors.clear();

    }
    inline void SetEdgesContainer(const  typename TEdgesContainer::Pointer & edgesContainer){
        m_EdgesContainer=edgesContainer;
    }


    typename TEdgesContainer::AJEdgeHandler GetEdgeFromActor(const vtkSmartPointer<vtkActor> & actor){

        auto it= m_ActorsToEdges.find(actor);
        assert(it!=m_ActorsToEdges.end());
        return it->second;
    }

    void HighlightEdge(const typename TEdgesContainer::AJEdgeHandler & edgeHandler){
        vtkSmartPointer<vtkActor> actor=m_EdgesToActors[edgeHandler];
        assert(actor);
        actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
        //actor->GetProperty()->SetDiffuse(1.0);
        //actor->GetProperty()->SetSpecular(0.0);
    }

    void DeemphasizeAJEdge(const typename TEdgesContainer::AJEdgeHandler & edgeHandler){
        vtkSmartPointer<vtkActor> actor=m_EdgesToActors[edgeHandler];
        assert(actor);
        actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    }

    virtual void Show(){
        for(auto it= m_ActorsToEdges.begin();it!=m_ActorsToEdges.end();it++){
            it->first->VisibilityOn();
        }
    }
    virtual void Hide(){

        for(auto it= m_ActorsToEdges.begin();it!=m_ActorsToEdges.end();it++){
            it->first->VisibilityOff();
        }
    }

    virtual  void PickOn(){
        for(auto it= m_ActorsToEdges.begin();it!=m_ActorsToEdges.end();it++){
            it->first->PickableOn();
        }
    }
    virtual  void PickOff(){
        for(auto it= m_ActorsToEdges.begin();it!=m_ActorsToEdges.end();it++){
            it->first->PickableOff();
        }
    }

    virtual vtkSmartPointer<vtkActor> DrawAJEdge(const typename TEdgesContainer::AJEdgeHandler & edgeHandler){

        const typename TEdgesContainer::AJVertexHandler source =this->m_EdgesContainer->GetAJEdgeSource(edgeHandler);
        const typename TEdgesContainer::AJVertexHandler target =this->m_EdgesContainer->GetAJEdgeTarget(edgeHandler);

        const typename TEdgesContainer::AJVertexType::Pointer  sourceVertex=this->m_EdgesContainer->GetAJVertex(source);
        const typename TEdgesContainer::AJVertexType::Pointer  targetVertex=this->m_EdgesContainer->GetAJVertex(target);

        vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();

        lineSource->SetPoint1(sourceVertex->GetPosition()[0],sourceVertex->GetPosition()[1],sourceVertex->GetPosition()[2]);
        lineSource->SetPoint2(targetVertex->GetPosition()[0],targetVertex->GetPosition()[1],targetVertex->GetPosition()[2]);
        lineSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

        mapper->SetInputData(lineSource->GetOutput());


        vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();

        lineActor->SetMapper(mapper);
        lineActor->VisibilityOff();
        lineActor->SetPickable(false);
        lineActor->GetProperty()->SetColor(0.0,1.0,0.0);

        this->m_Renderer->AddActor(lineActor);

        this->m_ActorsToEdges[lineActor]=edgeHandler;
        this->m_EdgesToActors[edgeHandler]=lineActor;


        return lineActor;

    }

    virtual void Draw(){
        this->Reset();

        for(auto it = m_EdgesContainer->EdgesBegin();it!=m_EdgesContainer->EdgesEnd();it++){
            this->DrawAJEdge(*it);
        }
    }

    virtual ~EdgesDrawer(){

    }
};



#endif // EDGESDRAWER_H
