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
#include <vtkTubeFilter.h>

#include <vtkActor.h>
#include <vtkProperty.h>
#include <itkFixedArray.h>

#include <boost/tuple/tuple.hpp>
#include <boost/bimap.hpp>

#include "Drawer.h"
#include <map>

template<class TEdgesContainer> class EdgesDrawer : public ttt::Drawer {
public:

private:

    typename TEdgesContainer::Pointer m_EdgesContainer;

	typedef boost::bimap<boost::bimaps::set_of<typename TEdgesContainer::AJEdgeHandler>, boost::bimaps::set_of<vtkSmartPointer<vtkActor> > > EdgeActorContainer;

	EdgeActorContainer m_EdgesToActors;
	typedef typename EdgeActorContainer::value_type EdgeActor;


    std::map<typename TEdgesContainer::AJEdgeHandler, itk::FixedArray<double,3> > m_Edges2Colors;


public:
    EdgesDrawer(){

    }

    void Reset(){

        for(auto it= m_EdgesToActors.begin();it!=m_EdgesToActors.end();++it){
            m_Renderer->RemoveActor(it->right);
        }


        m_EdgesToActors.clear();


    }
    inline void SetEdgesContainer(const  typename TEdgesContainer::Pointer & edgesContainer){
        m_EdgesContainer=edgesContainer;
    }


    typename TEdgesContainer::AJEdgeHandler GetEdgeFromActor(const vtkSmartPointer<vtkActor> & actor){

    	return m_EdgesToActors.right.at(actor);
#if 0
        auto it= m_EdgesToActors.rightfind(actor);
        assert(it!=m_ActorsToEdges.end());
        return it->second;
#endif
    }

    void HighlightEdge(const typename TEdgesContainer::AJEdgeHandler & edgeHandler){
        vtkSmartPointer<vtkActor> actor=m_EdgesToActors.left.at(edgeHandler);
        assert(actor);
        actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
        //actor->GetProperty()->SetDiffuse(1.0);
        //actor->GetProperty()->SetSpecular(0.0);
    }

    void DeemphasizeAJEdge(const typename TEdgesContainer::AJEdgeHandler & edgeHandler){
        vtkSmartPointer<vtkActor> actor=m_EdgesToActors.left.at(edgeHandler);
        assert(actor);
        auto color = this->m_Edges2Colors[edgeHandler];
        actor->GetProperty()->SetColor(color[0],color[1],color[2]);
    }

    virtual void Show(){
        for(auto it= m_EdgesToActors.begin();it!=m_EdgesToActors.end();it++){
            it->right->VisibilityOn();
        }
    }
    virtual void Hide(){

        for(auto it= m_EdgesToActors.begin();it!=m_EdgesToActors.end();it++){
            it->right->VisibilityOff();
        }
    }

    virtual  void PickOn(){
        for(auto it= m_EdgesToActors.begin();it!=m_EdgesToActors.end();it++){
            it->right->PickableOn();
        }
    }
    virtual  void PickOff(){
        for(auto it= m_EdgesToActors.begin();it!=m_EdgesToActors.end();it++){
            it->right->PickableOff();
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

        // Create a tube (cylinder) around the line
         vtkSmartPointer<vtkTubeFilter> tubeFilter =
           vtkSmartPointer<vtkTubeFilter>::New();
         tubeFilter->SetInputConnection(lineSource->GetOutputPort());
         tubeFilter->SetRadius(0.00025); //default is .5
         tubeFilter->SetNumberOfSides(50);
         tubeFilter->Update();



        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

        mapper->SetInputData(tubeFilter->GetOutput());


        vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();

        lineActor->SetMapper(mapper);
        lineActor->VisibilityOff();
        lineActor->SetPickable(false);
        itk::FixedArray<double,3> color;
        if(m_Edges2Colors.find(edgeHandler)==m_Edges2Colors.end()){
        	color[0]=(double)rand()/RAND_MAX;
        	color[1]=(double)rand()/RAND_MAX;
        	color[2]=(double)rand()/RAND_MAX;
            m_Edges2Colors[edgeHandler] =color;
        }else{
        	color=m_Edges2Colors[edgeHandler];
        }
        lineActor->GetProperty()->SetColor(color[0],color[1],color[2]);

        this->m_Renderer->AddActor(lineActor);


        this->m_EdgesToActors.insert(EdgeActor(edgeHandler,lineActor));


        return lineActor;

    }

    virtual itk::FixedArray<double,3> GetEdgeColor(const typename TEdgesContainer::AJEdgeHandler & edgeHandler){
    	return m_Edges2Colors[edgeHandler];
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
