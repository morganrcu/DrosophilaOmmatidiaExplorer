/*
 * AJGraphToCellGraph.hxx
 *
 *  Created on: Mar 24, 2015
 *      Author: morgan
 */

#ifndef AJGRAPHTOTISSUEDESCRIPTOR_HXX_
#define AJGRAPHTOTISSUEDESCRIPTOR_HXX_

#include <AJGraphToTissueDescriptor.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_cross.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_sparse_matrix.h>
#include <itkPoint.h>
#include <algorithm>
#include "S2Embedding.h"
#include "itkAzimuthElevationToCartesianTransform.h"
/**
 * \class CentroidGraphCalculator
 * TODO
 */

template<class InputGraph, class OutputGraph, class TissueDescriptorType, class EdgeIndexMap> class CentroidGraphCalculator: public boost::planar_face_traversal_visitor {
public:
	const static int NumDimensions = 3; //InputGraph::NumDimensions;
	/**
	 * TODO
	 * @param arg_g
	 * @param arg_dual_g
	 * @param arg_em
	 */

	/**
	 * TODO
	 */
	typedef typename boost::graph_traits<typename OutputGraph::BoostGraphType>::vertex_descriptor vertex_t;
	/**
	 * TODO
	 */
	typedef typename boost::graph_traits<typename InputGraph::BoostGraphType>::edge_descriptor edge_t;
	/**
	 * TODO
	 */
	typedef typename std::vector<vertex_t> vertex_vector_t;
	/**
	 * TODO
	 */
	typedef boost::iterator_property_map<typename vertex_vector_t::iterator, EdgeIndexMap> edge_to_face_map_t;

	CentroidGraphCalculator(typename InputGraph::Pointer & g, typename OutputGraph::Pointer & dual_g, typename TissueDescriptorType::Pointer & tissue,
			EdgeIndexMap em) :
			m_Graph(g), m_Dual(dual_g), m_TissueDescriptor(tissue), m_EM(em), m_EdgeToFaceVector(m_Graph->GetNumEdges(),
					boost::graph_traits<typename OutputGraph::BoostGraphType>::null_vertex()) {

		m_Total = -1;
		m_CurrentCellVertex = -1;
		m_CurrentCentroid.Fill(0);
		m_EdgeToFace = edge_to_face_map_t(m_EdgeToFaceVector.begin(), m_EM);

	}

	/**
	 * TODO
	 */
	void begin_face() {

		m_CurrentCell = OutputGraph::CellType::New();
		m_CurrentCell->SetPosition(m_CurrentCentroid);
		m_CurrentCentroid.Fill(0);

		this->m_CurrentCellVertex = m_Dual->AddCell(m_CurrentCell);

		m_Total = 0;
	}
	/**
	 * TODO
	 */
	void end_face() {
		for (int i = 0; i < NumDimensions; i++) {
			m_CurrentCentroid[i] /= m_Total;
		}
		m_CurrentCell->SetPosition(m_CurrentCentroid);

	}
	/**
	 * TODO
	 * @param v
	 */
	template<typename Vertex>
	void next_vertex(Vertex v) {
		this->m_Graph->GetAJVertex(v)->AddCell(m_CurrentCellVertex);
		for (int i = 0; i < NumDimensions; i++) {
			m_CurrentCentroid[i] += m_Graph->GetAJVertex(v)->GetPosition()[i]; //									boost::get(SkeletonPointPropertyTag<NumDimensions>(), m_Graph, v).position[i];
		}
		m_CurrentCell->AddAJVertexToPerimeter(v);

		m_Total++;
	}

	/**
	 * TODO
	 * @param e
	 */
	template<typename Edge>
	void next_edge(Edge e) {
		m_CurrentCell->AddAJEdgeToPerimeter(e);

		vertex_t existing_face = m_EdgeToFace[e];
		if (existing_face == boost::graph_traits<typename OutputGraph::BoostGraphType>::null_vertex()) {
			m_EdgeToFace[e] = m_CurrentCellVertex;
		} else {
			auto dualEdge = m_Dual->AddCellEdge(existing_face, m_CurrentCellVertex);
			m_TissueDescriptor->GetAJGraph()->GetAJEdge(e)->SetCellPair(std::pair<unsigned long, unsigned long>(existing_face, m_CurrentCellVertex));
			m_TissueDescriptor->AddAJEdgeToCellEdgeMapping(e, dualEdge);
		}
	}

private:

	/**
	 * TODO
	 */

	typename OutputGraph::CellType::Pointer m_CurrentCell;

	typename OutputGraph::CellVertexHandler m_CurrentCellVertex;

	/**
	 *
	 */
	itk::Point<double, NumDimensions> m_CurrentCentroid;
	/**
	 *
	 */
	int m_Total;

	/**
	 * TODO
	 */
	typename InputGraph::Pointer m_Graph;
	/**
	 * TODO
	 */
	typename OutputGraph::Pointer m_Dual;
	/**
	 *
	 */
	typename TissueDescriptorType::Pointer m_TissueDescriptor;
	/**
	 * TODO
	 */
	EdgeIndexMap m_EM;
	/**
	 * TODO
	 */
	//edge_to_face_map_t * m_EdgeToFace;
	edge_to_face_map_t m_EdgeToFace;
	/**
	 * TODO
	 */
	vertex_vector_t m_EdgeToFaceVector;
};

/**
 * TODO
 * @param g
 * @param dual_g
 * @param embedding
 * @param em
 */
template<typename InputGraph, typename OutputGraph, typename TissueDescriptor, typename PlanarEmbedding, typename EdgeIndexMap>
void create_dual_graph(typename InputGraph::Pointer & g, typename OutputGraph::Pointer & dual_g, typename TissueDescriptor::Pointer & tissue,
		PlanarEmbedding embedding, EdgeIndexMap em) {
	CentroidGraphCalculator<InputGraph, OutputGraph, TissueDescriptor, EdgeIndexMap> visitor(g, dual_g, tissue, em);
	boost::planar_face_traversal(g->GetBoostGraph(), embedding, visitor, em);
}

/**
 * TODO
 * @param g
 * @param dual_g
 * @param embedding
 */
template<typename InputGraph, typename OutputGraph, typename TissueDescriptorType, typename PlanarEmbedding>
void create_dual_graph(typename InputGraph::Pointer & g, typename OutputGraph::Pointer & dual_g, typename TissueDescriptorType::Pointer & tissue,
		PlanarEmbedding embedding) {
	auto edgeindexmap = boost::get(boost::edge_index, g->GetBoostGraph());
	create_dual_graph<InputGraph, OutputGraph, TissueDescriptorType, PlanarEmbedding, decltype(edgeindexmap)>(g, dual_g, tissue, embedding,
			edgeindexmap);
}

template<class TAJGraph, class TTissueDescriptor> void AJGraphToTissueDescriptor<TAJGraph, TTissueDescriptor>::Compute() {

	//1. Compute embedding of the AJGraph from R3 to S2

	{

		vnl_matrix<double> Dgeodesic(this->m_AJGraph->GetNumVertices(),this->m_AJGraph->GetNumVertices());

		typename boost::property_map<typename TAJGraph::BoostGraphType, boost::edge_weight_t>::type  weight_pmap = boost::get(boost::edge_weight_t(), this->m_AJGraph->GetBoostGraph());
		for(auto edgeIt = this->m_AJGraph->EdgesBegin();edgeIt!=this->m_AJGraph->EdgesEnd();++edgeIt){
			auto edgeHandler = *edgeIt;
			auto sourceHandler = this->m_AJGraph->GetAJEdgeSource(edgeHandler);
			auto targetHandler = this->m_AJGraph->GetAJEdgeTarget(edgeHandler);
			weight_pmap[edgeHandler] = (this->m_AJGraph->GetAJVertex(sourceHandler)->GetPosition()-this->m_AJGraph->GetAJVertex(targetHandler)->GetPosition()).GetNorm();
			double distance = (this->m_AJGraph->GetAJVertex(sourceHandler)->GetPosition()-this->m_AJGraph->GetAJVertex(targetHandler)->GetPosition()).GetNorm();
	  }
		boost::johnson_all_pairs_shortest_paths(this->m_AJGraph->GetBoostGraph(),Dgeodesic);

		std::vector<double> max(Dgeodesic.rows());
		for(int d =0;d<Dgeodesic.rows();d++){
			max[d]=Dgeodesic.get_row(d).max_value();
		}
		std::sort(max.begin(),max.end());
		double sphereRadius = max[max.size()/2];
		std::cout << sphereRadius << std::endl;

		Dgeodesic = Dgeodesic/sphereRadius;



		vnl_matrix<double> D(this->m_AJGraph->GetNumVertices(),this->m_AJGraph->GetNumVertices());
		D.fill(0);
		for(auto vertexIt = this->m_AJGraph->VerticesBegin();vertexIt!=this->m_AJGraph->VerticesEnd();++vertexIt){
			for(auto vertex2It = this->m_AJGraph->VerticesBegin();vertex2It!=this->m_AJGraph->VerticesEnd();++vertex2It){
				double distance = (this->m_AJGraph->GetAJVertex(*vertexIt)->GetPosition()-this->m_AJGraph->GetAJVertex(*vertex2It)->GetPosition()).GetNorm();
				D(*vertexIt,*vertex2It)=distance;
				D(*vertex2It,*vertexIt)=distance;
			}

		}
		int K=10;
		for (int d = 0; d < D.rows(); d++) {
			auto row = D.get_row(d);
			std::sort(row.begin(),row.end());
			double e = row[K];
			for(int dd=0;dd < D.cols();dd++){
				if(D(d,dd)>e){
					D(d,dd)=0;

				}
			}
		}
#if 0
		std::vector<double> max(D.rows());
		for (int d = 0; d < D.rows(); d++) {
			max[d] = D.get_row(d).max_value();
		}
		std::sort(max.begin(), max.end());
		double sphereRadius = max[max.size() / 2];
		std::cout << sphereRadius << std::endl;
#endif
		D = D / sphereRadius;

#if 0
		vnl_matrix<double> D(this->m_AJGraph->GetNumVertices(),this->m_AJGraph->GetNumVertices());
		vnl_matrix<double> Dgeodesic(this->m_AJGraph->GetNumVertices(),this->m_AJGraph->GetNumVertices());
		D.fill(0);
		typename boost::property_map<typename TAJGraph::BoostGraphType, boost::edge_weight_t>::type  weight_pmap = boost::get(boost::edge_weight_t(), this->m_AJGraph->GetBoostGraph());
		for(auto edgeIt = this->m_AJGraph->EdgesBegin();edgeIt!=this->m_AJGraph->EdgesEnd();++edgeIt){
			auto edgeHandler = *edgeIt;
			auto sourceHandler = this->m_AJGraph->GetAJEdgeSource(edgeHandler);
			auto targetHandler = this->m_AJGraph->GetAJEdgeTarget(edgeHandler);
			weight_pmap[edgeHandler] = (this->m_AJGraph->GetAJVertex(sourceHandler)->GetPosition()-this->m_AJGraph->GetAJVertex(targetHandler)->GetPosition()).GetNorm();
			double distance = (this->m_AJGraph->GetAJVertex(sourceHandler)->GetPosition()-this->m_AJGraph->GetAJVertex(targetHandler)->GetPosition()).GetNorm();
			D(sourceHandler,targetHandler)=distance;
			D(targetHandler,sourceHandler)=distance;
	  }
		boost::johnson_all_pairs_shortest_paths(this->m_AJGraph->GetBoostGraph(),Dgeodesic);
		std::vector<double> max(D.rows());
		for(int d =0;d<D.rows();d++){
			max[d]=Dgeodesic.get_row(d).max_value();
		}
		std::sort(max.begin(),max.end());
		double sphereRadius = max[max.size()/2];
		std::cout << sphereRadius << std::endl;

		Dgeodesic = Dgeodesic/sphereRadius;
		D=D/sphereRadius;
#endif
#if 0
	//

#if 0

#endif
	//vnl_matrix<double> W(this->m_AJGraph->GetNumVertices(),this->m_AJGraph->GetNumVertices());
	//vnl_matrix<double> W(this->m_AJGraph->GetNumVertices(),this->m_AJGraph->GetNumVertices());
	vnl_sparse_matrix<double> W(this->m_AJGraph->GetNumVertices(),this->m_AJGraph->GetNumVertices());
	//W.fill(0.0);
	//vnl_matrix<double> X(this->m_AJGraph->GetNumVertices(),3);
#if 0
	for(auto vertexIt = this->m_AJGraph->VerticesBegin();vertexIt!=this->m_AJGraph->VerticesEnd();++vertexIt){
		auto vertexPosition = this->m_AJGraph->GetAJVertex(*vertexIt)->GetPosition().GetVnlVector();
		X.set_row(*vertexIt,vertexPosition);
	}
#endif
	int K=10;
	for(auto vertexIt = this->m_AJGraph->VerticesBegin();vertexIt!=this->m_AJGraph->VerticesEnd();++vertexIt){

		int r=0;
		//vnl_matrix<double> X(this->m_AJGraph->AJVertexDegree(*vertexIt),3);
		vnl_vector<double> y = this->m_AJGraph->GetAJVertex(*vertexIt)->GetPosition().GetVnlVector();

		vnl_matrix<double> X(K,3);

		D(*vertexIt,*vertexIt)=10000000000;
		auto distances=D.get_row(*vertexIt);
		std::vector<unsigned> neighLocs;
		for(int k=0;k<K;k++){
			unsigned minLoc=distances.arg_min();
			distances[minLoc]=1000000000000000;
			auto vertexPosition = this->m_AJGraph->GetAJVertex(minLoc)->GetPosition().GetVnlVector();
			neighLocs.push_back(minLoc);
			X.set_row(r++,vertexPosition);
		}
#if 0
		for(auto neighIt = this->m_AJGraph->AdjacentAJVerticesBegin(*vertexIt);neighIt!=this->m_AJGraph->AdjacentAJVerticesEnd(*vertexIt);++neighIt){
			auto vertexPosition = this->m_AJGraph->GetAJVertex(*neighIt)->GetPosition().GetVnlVector();
			std::cout << vertexPosition << std::endl;
			X.set_row(r++,vertexPosition);
		}
#endif

		vnl_vector<double> alpha = vnl_matrix_inverse<double>(X*X.transpose())*X*y;
		alpha=alpha/alpha.one_norm();

		std::cout << "X" << std::endl;
		std::cout << X << std::endl;
		std::cout << "y" << std::endl;
		std::cout << y << std::endl;
		std::cout << "alpha" << std::endl;
		std::cout << alpha << std::endl;

#if 0
		if(this->m_AJGraph->AJVertexDegree(*vertexIt)==1){
			alpha(0)=1;
		}
		if(this->m_AJGraph->AJVertexDegree(*vertexIt)==2){
			alpha(0)=0.5;
			alpha(1)=0.5;
		}
#endif
		r=0;


		for(auto neigh : neighLocs){
			W(*vertexIt,neigh)=alpha[r++];
		}
#if 0

		for(auto neighIt = this->m_AJGraph->AdjacentAJVerticesBegin(*vertexIt);neighIt!=this->m_AJGraph->AdjacentAJVerticesEnd(*vertexIt);++neighIt){
			W(*vertexIt,*neighIt)=alpha[r++];
		}
#endif
		//std::cout << W.get_row(*vertexIt) << std::endl;
	}
#endif

	auto embedding=S2Embedding(Dgeodesic);
	embedding=S2Embedding(D,embedding);
	std::cout << embedding << std::endl;

	for(int v=0;v<D.rows();v++){
		itk::Point<double,3> sphericalPoint,cartesianPoint;
		//cartesianPoint[0]=embedding(v,0);
		//cartesianPoint[1]=embedding(v,1);
		//cartesianPoint[2]=embedding(v,2);
		cartesianPoint[0] = cos(embedding(v,0))*cos(embedding(v,1));
		cartesianPoint[1]= cos(embedding(v,0))*sin(embedding(v,1));
		cartesianPoint[2]= sin(embedding(v,0));
		sphericalPoint[0]=embedding(v,0);
		sphericalPoint[1]=embedding(v,1);
		sphericalPoint[2]=1;

		//cartesianPoint=azimuthElevation->TransformAzElToCartesian(sphericalPoint);
		this->m_AJGraph->GetAJVertex(v)->SetEmbeddedSphericalPosition(sphericalPoint);
		this->m_AJGraph->GetAJVertex(v)->SetEmbeddedPosition(cartesianPoint);
	}




	//sphrad = median(max(dfull)) / pi;
	//clear dfull

	}
	//
	typename boost::property_map<typename TAJGraph::BoostGraphType, boost::edge_index_t>::type e_index = boost::get(boost::edge_index,
			m_AJGraph->GetBoostGraph());
	//typename boost::property_map<CellGraph, EllipsePropertyTag>::type name = boost::get(EllipsePropertyTag(), m_TissueDescriptor->m_CellGraph);
	typename boost::graph_traits<typename TAJGraph::BoostGraphType>::edges_size_type edge_count = 0;

	typename boost::graph_traits<typename TAJGraph::BoostGraphType>::edge_iterator ei, ei_end;

	for (boost::tie(ei, ei_end) = boost::edges(m_AJGraph->GetBoostGraph()); ei != ei_end; ++ei) {
		boost::put(e_index, *ei, edge_count++);
	}

	//std::cout << "Num edges: " << edge_count << std::endl;

	typedef std::vector<typename boost::graph_traits<typename TAJGraph::BoostGraphType>::edge_descriptor> vec_t;

	std::vector<vec_t> embedding(boost::num_vertices(m_AJGraph->GetBoostGraph()));

	std::vector<vec_t> myembedding(boost::num_vertices(m_AJGraph->GetBoostGraph()));

	if (boost::boyer_myrvold_planarity_test(boost::boyer_myrvold_params::graph = m_AJGraph->GetBoostGraph(), boost::boyer_myrvold_params::embedding =
			&embedding[0]))
		std::cout << "Input graph is planar" << std::endl;
	else {
		std::cout << "Input graph is not planar" << std::endl;
		return;
	}

	//TODO create my embedding

	for (auto it = m_AJGraph->VerticesBegin(); it != m_AJGraph->VerticesEnd(); it++) {
		typename TAJGraph::AJVertexHandler center = *it;
		//std::cout << "Embedding " << center << " " << m_AJGraph->AJVertexDegree(center) << std::endl;

		if (m_AJGraph->AJVertexDegree(center) < 3) {
			typename boost::graph_traits<typename TAJGraph::BoostGraphType>::out_edge_iterator oi, oi_end;

			for (boost::tie(oi, oi_end) = boost::out_edges(center, m_AJGraph->GetBoostGraph()); oi != oi_end; ++oi) {
				myembedding[center].push_back(*oi);
			}

		} else {

			typedef std::pair<double, typename TAJGraph::AJVertexHandler> AngleAndVertex;

			std::vector<AngleAndVertex> angles;

			typename boost::graph_traits<typename TAJGraph::BoostGraphType>::adjacency_iterator ai, ai_end;
			boost::tie(ai, ai_end) = boost::adjacent_vertices(center, m_AJGraph->GetBoostGraph());

			auto reference = *ai;

			auto referencePosition = m_AJGraph->GetAJVertex(reference)->GetPosition();
			auto centerPosition = m_AJGraph->GetAJVertex(center)->GetPosition();

			auto referenceVector = referencePosition - centerPosition;

			angles.push_back(AngleAndVertex(0, reference));				//FIRST NEIGHBOR
			++ai;

			//std::cout << "Center: " << centerPosition << "\t Reference: " << referenceVector << std::endl;
			while (ai != ai_end) {

				auto current = *ai;
				auto currentPosition = m_AJGraph->GetAJVertex(current)->GetPosition();

				auto currentVector = currentPosition - centerPosition;

				double dot = referenceVector * currentVector;
				//double angle = atan2(itk::CrossProduct(referenceVector,currentVector).GetNorm(),dot);
				//double angle = acos(referenceVector*currentVector / (referenceVector.GetNorm()*currentVector.GetNorm()));
				double angle = atan2(currentVector[1], currentVector[0]) - atan2(referenceVector[1], referenceVector[0]);
				//double sign = dot/fabs(dot);
				angles.push_back(AngleAndVertex(angle, current));				//FIRST NEIGHBOR

				++ai;
				//std::cout <<"\t" << currentVector << "\t" << angle << std::endl;
			}
			assert(ai == ai_end);

			std::sort(angles.begin(), angles.end());

			for (auto anglesIt = angles.begin(); anglesIt != angles.end(); ++anglesIt) {
				//std::cout << "\t" << center << " To " << anglesIt->second << " angle " << anglesIt->first << std::endl;
				myembedding[center].push_back(boost::edge(center, anglesIt->second, m_AJGraph->GetBoostGraph()).first);
			}

#if 0
			typedef typename TAJGraph::AJVertexType::PointType PointType;
			std::vector<PointType > points,centeredPoints;
			std::vector<PointType> projectedPoints;
			PointType meanPoint;
			meanPoint.Fill(0);

			PointType center=m_AJGraph->GetAJVertex(v)->GetPosition();
			std::cout << "Center: " << center << std::endl;
			points.push_back(center);

			for(int d=0;d<3;d++) {
				meanPoint[d]+=center[d];
			}

			int total=1;

			typename boost::graph_traits<typename TAJGraph::BoostGraphType>::adjacency_iterator ai, ai_end;

			for (boost::tie(ai, ai_end) = boost::adjacent_vertices(v,m_AJGraph->GetBoostGraph()); ai != ai_end; ++ai) {

				auto position = m_AJGraph->GetAJVertex(*ai)->GetPosition();
				std::cout << position << std::endl;

				points.push_back(position);

				for(int d=0;d<3;d++) {
					meanPoint[d]+=position[d];
				}

				total++;
			}
			for(int d=0;d<3;d++) {
				meanPoint[d]/=total;
			}

			vnl_matrix<double> A(total, 3);
			//std::cout << "Mean: " << meanPoint << std::endl;
			int row = 0;

			for (auto it = points.begin(); it != points.end(); ++it) {

				PointType diff = *it - meanPoint;
				centeredPoints.push_back(diff);
				A(row, 0) = diff[0];
				A(row, 1) = diff[1];
				A(row, 2) = diff[2];
				row++;
			}

			//std::cout << "A" << std::endl;
			//std::cout << A << std::endl;

			vnl_matrix<double> S = A.transpose() * A;

			//std::cout << "S" << std::endl;
			//std::cout << S << std::endl;

			vnl_symmetric_eigensystem<double> eigensystem(S);
			vnl_vector<double> supportingPlane = eigensystem.get_eigenvector(0);
			//double sign=vnl_determinant(eigensystem.V);
			double sign = supportingPlane(2) / std::abs(supportingPlane(2));

			std::cout << supportingPlane << " " << sign << std::endl;
			if(supportingPlane(2)<0) {
				supportingPlane=-supportingPlane;
			}

			for (auto it = centeredPoints.begin(); it != centeredPoints.end(); ++it) {
				PointType a = *it;
				PointType proj;

				double dot = a[0] * supportingPlane[0] + a[1] * supportingPlane[1] + a[2] * supportingPlane[2];
				proj[0] = a[0] - dot * supportingPlane[0];
				proj[1] = a[1] - dot * supportingPlane[1];
				proj[2] = a[2] - dot * supportingPlane[2];
				projectedPoints.push_back(proj);

			}

			std::vector<vnl_vector<double> > transformedPoints;

			for (auto it = projectedPoints.begin(); it != projectedPoints.end(); ++it) {

				PointType a = *it;
				vnl_vector<double> vec(3);
				vec(0) = a[0];
				vec(1) = a[1];
				vec(2) = a[2];

				vnl_vector<double> ap = eigensystem.V.transpose() * vec;
				vnl_vector<double> transformed(2);
				transformed(0) = ap[1];
				transformed(1) = ap[2];
				transformedPoints.push_back(transformed);

			}

			std::vector<vnl_vector<double> >::iterator it = transformedPoints.begin();

			vnl_vector<double> projectedCenter = *it;
			it++;

			vnl_vector<double> reference = *it;
			it++;

			typedef std::pair<double, typename TAJGraph::AJVertexHandler> AngleAndVertex;

			std::vector<AngleAndVertex> angles;

			boost::tie(ai, ai_end) = boost::adjacent_vertices(v, m_AJGraph->GetBoostGraph());

			angles.push_back(AngleAndVertex(0, *ai));				//FIRST NEIGHBOR
			++ai;

			vnl_vector<double> referenceVector = reference - projectedCenter;

			while (it != transformedPoints.end()) {

				vnl_vector<double> currentVector = *it - projectedCenter;

				double angle = atan2(currentVector(1), currentVector(0)) - atan2(referenceVector(1), referenceVector(0));
				angles.push_back(AngleAndVertex(-sign*angle, *ai));				//FIRST NEIGHBOR

				++ai;
				++it;
			}
			assert(it == transformedPoints.end());
			assert(ai == ai_end);

			std::sort(angles.begin(),angles.end());

			for(typename std::vector<AngleAndVertex>::iterator it = angles.begin();it!=angles.end();++it) {
				//std::cout << v << " To " << it->second << " angle " << it->first << std::endl;
				myembedding[v].push_back(boost::edge(v,it->second,m_AJGraph->GetBoostGraph()).first);
			}

			assert(embedding[v].size()==myembedding[v].size());
			for(int i=0;i<embedding[v].size();i++) {
				std::cout << embedding[v][i] << " " << myembedding[v][i] << std::endl;
			}
#endif
		}
		assert(embedding[center].size() == myembedding[center].size());
		for (int i = 0; i < embedding[center].size(); i++) {
			//std::cout << embedding[center][i] << " " << myembedding[center][i] << std::endl;
		}
	}

	m_TissueDescriptor = TissueDescriptorType::New();
	m_TissueDescriptor->SetAJGraph(this->m_AJGraph);
	auto cellGraph = CellGraphType::New();

	create_dual_graph<AJGraphType, CellGraphType, TissueDescriptorType, decltype(&myembedding[0])>(m_AJGraph, cellGraph, m_TissueDescriptor,
			&myembedding[0]);

	for (auto it = cellGraph->CellEdgesBegin(); it != cellGraph->CellEdgesEnd(); it++) {
		std::cout << *it << " " << boost::get(boost::edge_index, cellGraph->GetBoostGraph(), *it) << std::endl;
	}

	{
		int maxDegree = -1;
		typename CellGraphType::CellVertexHandler outFaceHdlr;

		for (auto it = cellGraph->CellsBegin(); it != cellGraph->CellsEnd(); ++it) {
			int currentDegree = cellGraph->CellVertexDegree(*it);
			if (currentDegree > maxDegree) {
				maxDegree = currentDegree;
				outFaceHdlr = *it;

			}
		}
		std::cout << "Outer face: " << outFaceHdlr << std::endl;
		cellGraph->SetUnboundedFace(outFaceHdlr);
		auto outFace = cellGraph->GetCell(outFaceHdlr);
		for (auto it = outFace->BeginPerimeterAJVertices(); it != outFace->EndPerimeterAJVertices(); ++it) {
			m_TissueDescriptor->AddAJVertexToTissuePerimeter(*it);
		}
		for (auto it = outFace->BeginPerimeterAJEdges(); it != outFace->EndPerimeterAJEdges(); ++it) {
			m_TissueDescriptor->AddAJEdgeToTissuePerimeter(*it);
		}

	}
	m_TissueDescriptor->SetCellGraph(cellGraph);

	{
		for (auto it = cellGraph->CellsBegin(); it != cellGraph->CellsEnd(); ++it) {
			int total = 0;
			typedef itk::Point<double, 3> PointType;
			std::vector<PointType> points, centeredPoints;
			PointType meanPoint;
			meanPoint.Fill(0);
			auto cell = cellGraph->GetCell(*it);
			for (auto perimeterIt = cell->BeginPerimeterAJVertices(); perimeterIt != cell->EndPerimeterAJVertices(); ++perimeterIt) {
				itk::Point<double, 3> position = m_AJGraph->GetAJVertex(*perimeterIt)->GetPosition();
				points.push_back(position);
				meanPoint[0] += position[0];
				meanPoint[1] += position[1];
				meanPoint[2] += position[2];
				total++;
			}

			meanPoint[0] /= total;
			meanPoint[1] /= total;
			meanPoint[2] /= total;

			//std::cout << "MeanPoint " << meanPoint << std::endl;
			vnl_matrix<double> A(total, 3);

			int row = 0;

			for (std::vector<PointType>::iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt) {
				PointType diff = *pointIt - meanPoint;
				centeredPoints.push_back(diff);
				A(row, 0) = diff[0];
				A(row, 1) = diff[1];
				A(row, 2) = diff[2];
				row++;

			}
			//std::cout << row << std::endl;
			//assert(row>1);

			vnl_matrix_fixed<double, 3, 3> S = A.transpose() * A; //FEATURE: Scatter Matrix

			vnl_symmetric_eigensystem<double> eigensystem(S);

			vnl_vector_fixed<double, 3> normalPlane = eigensystem.get_eigenvector(0); //FEATURE: Supporting plane

			typedef itk::Vector<double, 3> VectorType;
			VectorType normalVector;
			normalVector.SetVnlVector(normalPlane);
			//cellNormal.SetValue(normalVector);

			std::vector<PointType> projectedPoints;
			//std::cout << "Projections" << std::endl;
			for (std::vector<PointType>::iterator pointIt = centeredPoints.begin(); pointIt != centeredPoints.end(); ++pointIt) {

				PointType a = *pointIt;
				PointType proj;

				double dot = a[0] * normalPlane[0] + a[1] * normalPlane[1] + a[2] * normalPlane[2];
				proj[0] = a[0] - dot * normalPlane[0];
				proj[1] = a[1] - dot * normalPlane[1];
				proj[2] = a[2] - dot * normalPlane[2];
				projectedPoints.push_back(proj);
				//std::cout << proj << std::endl;
			}

			std::vector<vnl_vector<double> > transformedPoints;

			//std::cout << "Transformed" << std::endl;

			for (std::vector<PointType>::iterator pointIt = projectedPoints.begin(); pointIt != projectedPoints.end(); ++pointIt) {

				PointType a = *pointIt;
				vnl_vector<double> vec(3);
				vec(0) = a[0];
				vec(1) = a[1];
				vec(2) = a[2];

				vnl_vector<double> ap = eigensystem.V.transpose() * vec;
				vnl_vector<double> transformed(2);
				transformed(0) = ap[1];
				transformed(1) = ap[2];
				transformedPoints.push_back(transformed);
				//std::cout << transformed << std::endl;

			}

			transformedPoints.push_back(transformedPoints[0]);

			vnl_vector<double> centroid2D(2);

			centroid2D.fill(0);
			double perimeter = 0;
			double area = 0;

			double xx = 0;
			double xy = 0;
			double yy = 0;

			for (int i = 0; i < transformedPoints.size() - 1; i++) {
				perimeter += (transformedPoints[i + 1] - transformedPoints[i]).magnitude();

				double common = (transformedPoints[i](0) * transformedPoints[i + 1](1) - transformedPoints[i + 1](0) * transformedPoints[i](1));

				area += common;
				centroid2D(0) += (transformedPoints[i](0) + transformedPoints[i + 1](0)) * common;
				centroid2D(1) += (transformedPoints[i](1) + transformedPoints[i + 1](1)) * common;
				xx += (pow(transformedPoints[i](0), 2) + transformedPoints[i](0) * transformedPoints[i + 1](0) + pow(transformedPoints[i + 1](0), 2))
						* common;
				yy += (pow(transformedPoints[i](1), 2) + transformedPoints[i](1) * transformedPoints[i + 1](1) + pow(transformedPoints[i + 1](1), 2))
						* common;
				xy += (2 * transformedPoints[i](0) * transformedPoints[i](1) + transformedPoints[i](0) * transformedPoints[i + 1](1)
						+ transformedPoints[i + 1](0) * transformedPoints[i](1)) * common;
				//std::cout << common << std::endl;
			}

			area = fabs(area / 2);

			cell->SetArea(area);

			//std::cout << "Area:" << area << std::endl;
			//std::cout << "Perimeter:" << perimeter << std::endl;

			cell->SetPerimeterLength(fabs(perimeter));

			centroid2D(0) = centroid2D(0) / (6 * area);
			centroid2D(1) = centroid2D(1) / (6 * area);
			//std::cout << "Centroid2D" << std::endl;
			//std::cout<< centroid2D << std::endl;

			xx = fabs(xx / (12 * area));
			yy = fabs(yy / (12 * area));
			xy = xy / (24 * area);
			//std::cout << "XX:" << xx << std::endl;
			//std::cout << "XY:" << xy << std::endl;
			//std::cout << "YY:" << yy << std::endl;

			cell->SetXX(xx);
			cell->SetYY(yy);
			cell->SetXY(xy);

			vnl_vector<double> cp(3);
			cp(0) = 0;
			cp(1) = centroid2D(0);
			cp(2) = centroid2D(1);

			vnl_vector<double> cu = eigensystem.V * cp;

			//std::cout << "CU" << std::endl;
			//std::cout << cu << std::endl;

			PointType cg;
			PointType centroid3D;

			centroid3D[0] = cu[0] + meanPoint[0];
			centroid3D[1] = cu[1] + meanPoint[1];
			centroid3D[2] = cu[2] + meanPoint[2];

			//std::cout << "Centroid3D " << centroid3D << " "<< area << std::endl;

			cell->SetPosition(centroid3D);
		}
	}
}

#endif /* AJGRAPHTOTISSUEDESCRIPTOR_HXX_ */
