/*
 * AJGraphToCellGraph.h
 *
 *  Created on: Mar 23, 2015
 *      Author: morgan
 */

#ifndef AJGRAPHTOTISSUEDESCRIPTOR_H_
#define AJGRAPHTOTISSUEDESCRIPTOR_H_

#include "AJGraph.h"
#include "CellGraph.h"
#include <itkObject.h>
template<class TAJGraph,class TTissueDescriptor> class AJGraphToTissueDescriptor : public itk::Object{
public:
	typedef AJGraphToTissueDescriptor Self;
	typedef itk::SmartPointer<Self> Pointer;

	typedef TAJGraph AJGraphType;
	typedef TTissueDescriptor TissueDescriptorType;
	typedef typename TissueDescriptorType::CellGraphType CellGraphType;

	itkNewMacro(Self)
	itkGetObjectMacro(AJGraph,TAJGraph);
	itkSetObjectMacro(AJGraph,TAJGraph);

	itkGetObjectMacro(TissueDescriptor,TTissueDescriptor);
	itkSetObjectMacro(TissueDescriptor,TTissueDescriptor);


	typedef std::vector<typename TAJGraph::AJVertexHandler> PerimeterVerticesContainer;
	typedef std::vector<typename TAJGraph::AJEdgeHandler> PerimeterEdgesContainer;
private:
	typename TAJGraph::Pointer m_AJGraph;
	typename TTissueDescriptor::Pointer m_TissueDescriptor;
public:

	void Compute();

};
#include "AJGraphToTissueDescriptor.hxx"


#endif /* AJGRAPHTOCELLGRAPH_H_ */
