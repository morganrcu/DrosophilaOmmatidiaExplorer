/*
 * VertexMotionsDrawer.h
 *
 *  Created on: Mar 3, 2015
 *      Author: morgan
 */

#ifndef VERTEXMOTIONSDRAWER_H_
#define VERTEXMOTIONSDRAWER_H_



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
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <itkFixedArray.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <boost/tuple/tuple.hpp>

#include "Drawer.h"
#include <map>

template<class TVerticesContainer> class VertexMotionsDrawer : public ttt::Drawer {
public:

private:
    typedef itk::FixedArray<float,3> SpacingType;

    typename TVerticesContainer::Pointer m_VertexLocations;

    std::vector<vtkSmartPointer<vtkActor> > m_Actors;
public:
	static VertexMotionsDrawer* New(){
		return new VertexMotionsDrawer;
	}


	VertexMotionsDrawer(){
        //m_Spacing.Fill(1);
    }

    void Reset(){

        for(auto it= m_Actors.begin();it!=m_Actors.end();++it){
            m_Renderer->RemoveActor(*it);
    	}
        m_Actors.clear();

    }
    inline void SetVertexLocations(const  typename TVerticesContainer::Pointer & vertexLocations){
    	m_VertexLocations=vertexLocations;
    }


    virtual void Show(){
        for(auto it= m_Actors.begin();it!=m_Actors.end();it++){
            (*it)->VisibilityOn();
    	}
    }
    virtual void Hide(){
        for(auto it= m_Actors.begin();it!=m_Actors.end();it++){
        	(*it)->VisibilityOff();
    	}
    }

    virtual vtkSmartPointer<vtkActor> DrawAJVertex(const typename TVerticesContainer::AJVertexHandler & vertexHandler){

    	  //Create an arrow.
    	  vtkSmartPointer<vtkArrowSource> arrowSource =   vtkSmartPointer<vtkArrowSource>::New();
        // Compute a basis
        double normalizedX[3];
        double normalizedY[3];
        double normalizedZ[3];

        auto origin = this->m_VertexLocations->GetAJVertex(vertexHandler)->GetPosition();
        auto velocity = this->m_VertexLocations->GetAJVertex(vertexHandler)->GetVelocity();

        double length = velocity.GetNorm();
        length=length*5;
        velocity.Normalize();

        // The Z axis is an arbitrary vector cross X
        double arbitrary[3];
        arbitrary[0] = vtkMath::Random(-10,10);
        arbitrary[1] = vtkMath::Random(-10,10);
        arbitrary[2] = vtkMath::Random(-10,10);
        normalizedX[0]=velocity[0];
        normalizedX[1]=velocity[1];
        normalizedX[2]=velocity[2];
        vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
        vtkMath::Normalize(normalizedZ);


        // The Y axis is Z cross X
        vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
        vtkSmartPointer<vtkMatrix4x4> matrix =
          vtkSmartPointer<vtkMatrix4x4>::New();

        // Create the direction cosine matrix
        matrix->Identity();
        for (unsigned int i = 0; i < 3; i++)
          {
          matrix->SetElement(i, 0, normalizedX[i]);
          matrix->SetElement(i, 1, normalizedY[i]);
          matrix->SetElement(i, 2, normalizedZ[i]);
          }



        // Apply the transforms
        vtkSmartPointer<vtkTransform> transform =
          vtkSmartPointer<vtkTransform>::New();
        double startPoint[3];
        startPoint[0]=origin[0];
        startPoint[1]=origin[1];
        startPoint[2]=origin[2];
        transform->Translate(startPoint);
        transform->Concatenate(matrix);
        transform->Scale(length, length, length);

        // Transform the polydata
        vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
          vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformPD->SetTransform(transform);
        transformPD->SetInputConnection(arrowSource->GetOutputPort());

        //Create a mapper and actor for the arrow
        vtkSmartPointer<vtkPolyDataMapper> mapper =
          vtkSmartPointer<vtkPolyDataMapper>::New();
        vtkSmartPointer<vtkActor> actor =
          vtkSmartPointer<vtkActor>::New();
      #ifdef USER_MATRIX
        mapper->SetInputConnection(arrowSource->GetOutputPort());
        actor->SetUserMatrix(transform->GetMatrix());
      #else
        mapper->SetInputConnection(transformPD->GetOutputPort());
      #endif
        actor->GetProperty()->SetColor(255.0,0.0,0.0);
        actor->SetMapper(mapper);
        this->m_Renderer->AddActor(actor);


        return actor;

    }


    virtual void Draw(){
        this->Reset();
        for (auto it = m_VertexLocations->VerticesBegin(); it != m_VertexLocations->VerticesEnd(); it++) {
            this->m_Actors.push_back(this->DrawAJVertex(*it));

        }

    }

	virtual ~VertexMotionsDrawer(){

	}
};



#endif /* VERTEXMOTIONSDRAWER_H_ */
