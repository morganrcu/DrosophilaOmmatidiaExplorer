/*
 * CellVertex.h
 *
 *  Created on: Mar 23, 2015
 *      Author: morgan
 */

#ifndef CELL_H_
#define CELL_H_

#include <itkDataObject.h>
#include <itkMacro.h>
#include <itkObjectFactory.h>
#include <itkPoint.h>
#include <itkArray.h>
template<int dim,class TVertexHandler, class TEdgeHandler> class Cell: public itk::DataObject {

public:
	typedef Cell Self;
	typedef itk::DataObject Superclass;
	typedef itk::SmartPointer<Self> Pointer;

	typedef itk::Point<double, dim> PointType;
	typedef itk::Vector<double, dim> VectorType;
	typedef itk::Array<double> DescriptorType;

	itkNewMacro(Self)

	itkGetMacro(Position,PointType)
	itkSetMacro(Position,PointType)

	itkGetMacro(MedioapicalDescriptor,DescriptorType)
	itkSetMacro(MedioapicalDescriptor,DescriptorType)

	itkGetMacro(Velocity,VectorType)
	itkSetMacro(Velocity,VectorType)
	enum CellType {
		UNKNOWN = 0,
		ANTERIOR_CONE = 1,
		POSTERIOR_CONE = 2,
		POLAR_CONE = 3,
		EQUATORIAL_CONE = 4,
		PRIMARY = 5,
		LONG_SECUNDARY = 6,
		SHORT_SECUNDARY = 7,
		TERTIARY = 8,
		BRISTLE = 9,
		SURVIVING = 10,
		APOPTOTIC = 11
	};

	itkGetMacro(CellType,CellType)
	itkSetMacro(CellType,CellType)

	itkGetMacro(Area,double)
	;itkSetMacro(Area,double)
	;

	itkGetMacro(XX,double)
	;itkSetMacro(XX,double)
	;

	itkGetMacro(XY,double)
	;itkSetMacro(XY,double)
	;

	itkGetMacro(YY,double)
	;itkSetMacro(YY,double)
	;

	itkGetMacro(PerimeterLength,double)
	;itkSetMacro(PerimeterLength,double)
	;

	itkGetMacro(Eccentricity,double)
	;itkSetMacro(Eccentricity,double)
	;

	typedef std::list<TVertexHandler> PerimeterVertexContainer;
	typedef typename PerimeterVertexContainer::iterator PerimeterVertexIterator;

	typedef std::list<TEdgeHandler> PerimeterEdgeContainer;
	typedef typename PerimeterEdgeContainer::iterator PerimeterEdgeIterator;

	void AddAJVertexToPerimeter(const TVertexHandler & v) {
		m_PerimeterVertices.push_back(v);
	}

	void AddAJEdgeToPerimeter(const TEdgeHandler & e) {
		m_PerimeterEdges.push_back(e);
	}

	PerimeterVertexIterator BeginPerimeterAJVertices() {
		return m_PerimeterVertices.begin();
	}
	PerimeterVertexIterator EndPerimeterAJVertices() {
		return m_PerimeterVertices.end();
	}

	PerimeterEdgeIterator BeginPerimeterAJEdges() {
		return m_PerimeterEdges.begin();
	}
	PerimeterEdgeIterator EndPerimeterAJEdges() {
		return m_PerimeterEdges.end();
	}

protected:
	Cell() {
		m_Position.Fill(0);
		m_Velocity.Fill(0);
		m_XX = 0;
		m_XY = 0;
		m_YY = 0;
		m_Area = 0;
		m_PerimeterLength = 0;
		m_Eccentricity = 0;
		m_Skewness = 0;
		m_PerimeterVertices.clear();
		m_PerimeterEdges.clear();
		m_CellType = UNKNOWN;
	}

private:
	CellType m_CellType;
	PointType m_Position;

	double m_XX;
	double m_YY;
	double m_XY;
	double m_Area;
	double m_PerimeterLength;
	double m_Eccentricity;
	double m_Skewness;

	DescriptorType m_MedioapicalDescriptor;
	VectorType m_Velocity;

	PerimeterVertexContainer m_PerimeterVertices;
	PerimeterEdgeContainer m_PerimeterEdges;

};

#endif /* CELLVERTEX_H_ */
