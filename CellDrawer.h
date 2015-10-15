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
#ifndef TRACKINGDRAWER_H_
#define TRACKINGDRAWER_H_
#include <vtkPolygon.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkTriangleFilter.h>
#include <boost/bimap.hpp>
#include "Drawer.h"
#include <vtkColor.h>
template<class CellContainer, class AJVerticesContainer> class CellDrawer: public ttt::Drawer {

	typedef typename CellContainer::CellVertexHandler CellVertexHandler;
	typedef boost::bimap<boost::bimaps::set_of<CellVertexHandler>, boost::bimaps::set_of<vtkSmartPointer<vtkActor> > > CellActorContainer;

	typedef typename CellActorContainer::value_type CellActor;
	CellActorContainer m_CellToActor;


public:
	CellDrawer() {

	}
	virtual ~CellDrawer() {

	}

	inline void SetCells(const typename CellContainer::Pointer & cells) {
		m_Cells = cells;
	}

	inline void SetAJVertices(const typename AJVerticesContainer::Pointer & vertices){
		m_AJVertices=vertices;
	}
	virtual void Reset() {
		std::for_each(m_CellToActor.begin(),m_CellToActor.end(),[&](CellActor & cellActor){m_Renderer->RemoveActor(cellActor.right);});
		m_CellToActor.clear();
	}
	virtual void PickOn(){
		std::for_each(m_CellToActor.begin(),m_CellToActor.end(),[&](CellActor & cellActor){cellActor.right->PickableOn();});

	}
	virtual void PickOff(){
		std::for_each(m_CellToActor.begin(),m_CellToActor.end(),[&](CellActor & cellActor){cellActor.right->PickableOff();});
	}

	virtual CellVertexHandler GetCellFromActor(const vtkSmartPointer<vtkActor> & actor){

        return m_CellToActor.right.at(actor);

   	}

    void HighlightCell(const CellVertexHandler & cellHandler){
        vtkSmartPointer<vtkActor> actor=m_CellToActor.left.at(cellHandler);
        assert(actor);
        actor->GetProperty()->SetColor(1.0, 1.0, 1.0);
    }

    void DeemphasizeCell(const CellVertexHandler & cellHandler){
        vtkSmartPointer<vtkActor> actor=m_CellToActor.left.at(cellHandler);
        assert(actor);
        std::vector<double> color = this->GetCellColor(cellHandler);
        actor->GetProperty()->SetColor(color[0] ,color[1], color[2]);

    }

	virtual void DrawCell(const typename CellContainer::CellVertexHandler & cellHandler){
		auto cell = m_Cells->GetCell(cellHandler);

		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		int npoints = 0;

		for (auto perimeterIt = cell->BeginPerimeterAJVertices(); perimeterIt != cell->EndPerimeterAJVertices(); ++perimeterIt) {
			typename AJVerticesContainer::AJVertexType::Pointer vertex = m_AJVertices->GetAJVertex(*perimeterIt);
			auto position = vertex->GetPosition();
			points->InsertNextPoint(position[0], position[1], position[2]);
			npoints++;
		}
		if(npoints>1){
			vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
			polygon->GetPointIds()->SetNumberOfIds(npoints); //make a quad
			for (int i = 0; i < npoints; i++) {
				polygon->GetPointIds()->SetId(i, i);
			}

			// Add the polygon to a list of polygons
			vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
			polygons->InsertNextCell(polygon);

			// Create a PolyData
			vtkSmartPointer<vtkPolyData> polygonPolyData = vtkSmartPointer<vtkPolyData>::New();
			polygonPolyData->SetPoints(points);
			polygonPolyData->SetPolys(polygons);

			vtkSmartPointer<vtkTriangleFilter> triangleFilter =   vtkSmartPointer<vtkTriangleFilter>::New();
			triangleFilter->SetInputData(polygonPolyData);
			triangleFilter->Update();

						// Create a mapper and actor
			vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

			mapper->SetInputData(triangleFilter->GetOutput());

			vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

			actor->SetMapper(mapper);

			std::vector<double> color = this->GetCellColor(cellHandler);

			actor->GetProperty()->SetColor(color[0] ,color[1], color[2]);
			m_Renderer->AddActor(actor);
			m_CellToActor.insert(CellActor(cellHandler,actor));
		}else{
			auto perimeterIt = cell->BeginPerimeterAJVertices();
			typename AJVerticesContainer::AJVertexType::Pointer vertex = m_AJVertices->GetAJVertex(*perimeterIt);
			auto position = vertex->GetPosition();

			vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
			sphere->SetCenter(position[0], position[1], position[2]);
			sphere->SetRadius(0.0005);
			sphere->Update();
			vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

			mapper->SetInputConnection(sphere->GetOutputPort());

			vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();

			actor->SetMapper(mapper);

			m_Renderer->AddActor(actor);
			m_CellToActor.insert(CellActor(cellHandler,actor));
		}



	}
	virtual void Draw() {
		this->Reset();
		for (auto it = m_Cells->CellsBegin(); it!=m_Cells->CellsEnd(); ++it) {
			if(*it!=m_Cells->GetUnboundedFace()){
				this->DrawCell(*it);
			}

		}
	}

	virtual void Show() {
		for (auto it = m_CellToActor.begin(); it != m_CellToActor.end(); it++) {
			it->right->VisibilityOn();
		}

	}
	virtual void Hide() {
		for (auto it = m_CellToActor.begin(); it != m_CellToActor.end(); it++) {
			it->right->VisibilityOff();
		}
	}
private:
	std::vector<double> GetCellColor(const typename CellContainer::CellVertexHandler & cellHandler){
		auto cell = m_Cells->GetCell(cellHandler);

		std::vector<double> color(3);
		switch(cell->GetCellType()){



		case CellContainer::CellType::UNKNOWN:
			color[0]=0.5;
			color[1]=0.5;
			color[2]=0.5;
			break;
		case CellContainer::CellType::ANTERIOR_CONE:
		case CellContainer::CellType::POSTERIOR_CONE:
		case CellContainer::CellType::POLAR_CONE:
		case CellContainer::CellType::EQUATORIAL_CONE:
			color[0]=0;
			color[1]=0;
			color[2]=1.0;
			break;
		case CellContainer::CellType::LONG_SECUNDARY:
		case CellContainer::CellType::SHORT_SECUNDARY:

			color[0]=1.0;
			color[1]=1.0;
			color[2]=0.0;
			break;
		case CellContainer::CellType::TERTIARY:

			color[0]=0.0;
			color[1]=1.0;
			color[2]=1.0;
			break;
		case CellContainer::CellType::BRISTLE:

			color[0]=0.0;
			color[1]=0.0;
			color[2]=1.0;
			break;
		case CellContainer::CellType::APOPTOTIC:
		case CellContainer::CellType::SURVIVING:

			color[0]=0.0;
			color[1]=1.0;
			color[2]=0.0;
			break;


		}
		return color;
	}
public:
	virtual void SetCellColor(const typename CellContainer::CellVertexHandler & cell, const vtkColor3d & color){
//		std::vector<double> color(3);

		this->m_CellToActor.left.at(cell)->GetProperty()->SetColor(color[0],color[1],color[2]);

	}
private:
	typedef std::vector<double> Color;

	std::map<int, Color> m_TrackID2Color;
	typename CellContainer::Pointer m_Cells;
	typename AJVerticesContainer::Pointer m_AJVertices;

};

#endif /* TRACKINGDRAWER_H_ */
/** @}*/
