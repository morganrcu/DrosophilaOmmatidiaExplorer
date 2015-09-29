/*
 * AJCorrespondence.h
 *
 *  Created on: Apr 15, 2015
 *      Author: morgan
 */

#ifndef AJCORRESPONDENCESET_H_
#define AJCORRESPONDENCESET_H_
#include <iostream>
//#include <boost/bimap.hpp>
//#include <boost/bimap/set_of.hpp>
//#include <boost/bimap/multiset_of.hpp>
//#include <boost/graph/adjacency_list.hpp>
#include <set>
#include <unordered_set>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>

using namespace ::boost;
using namespace ::boost::multi_index;
//#include <boost/graph/copy.hpp>
//#include <boost/graph/iteration_macros.hpp>

#if 0
class CorrespondenceCategory{

public:
	virtual ~CorrespondenceCategory(){

	}
};

template<class AJVertexDescriptor> class Entering : public CorrespondenceCategory{
private:
	AJVertexDescriptor m_V1;
public:

	Entering(){

	}
	void SetV1(const AJVertexDescriptor & v1){
		m_V1=v1;
	}
	AJVertexDescriptor GetV1(){
		return m_V1;
	}
};

template<class AJVertexDescriptor> class Leaving : public CorrespondenceCategory{
private:
	AJVertexDescriptor m_V0;
public:
	Leaving(){

	}
	void SetV0(const AJVertexDescriptor & v0){
		m_V0=v0;
	}
	AJVertexDescriptor GetV0() const{
		return m_V0;
	}
};

template<class AJVertexDescriptor> class Succesor : public CorrespondenceCategory{
private:
	AJVertexDescriptor m_V0;
	AJVertexDescriptor m_V1;

public:
	Succesor(){

	}
	void SetV0(const AJVertexDescriptor & v0){
		m_V0=v0;
	}
	void SetV1(const AJVertexDescriptor & v1){
		m_V1=v1;
	}
	AJVertexDescriptor GetV0() const{
		return m_V0;
	}
	AJVertexDescriptor GetV1() const{
		return m_V1;
	}
	bool operator==( const Succesor<AJVertexDescriptor> & other ) const{
		return m_V0==other.GetV0() && m_V1==other.GetV1();
	}

};

template<class AJVertexDescriptor> class Split : public CorrespondenceCategory {
private:
	AJVertexDescriptor m_V0;
	AJVertexDescriptor m_V1A;
	AJVertexDescriptor m_V1B;
public:
	Split(){

	}
	void SetV0(const AJVertexDescriptor & v0){
		m_V0=v0;
	}
	void SetV1A(const AJVertexDescriptor & v1a){
		m_V1A=v1a;
	}
	void SetV1B(const AJVertexDescriptor & v1b){
		m_V1B=v1b;
	}


	AJVertexDescriptor GetV0() const{
		return m_V0;
	}
	AJVertexDescriptor GetV1A() const{
		return m_V1A;
	}
	AJVertexDescriptor GetV1B() const{
		return m_V1B;
	}
	bool operator==(const Split & other) const{
			return m_V0==other.GetV0() && ( (m_V1A==other.GetV1A() && m_V1B==other.GetV1B()) || (m_V1A==other.GetV1B() && m_V1B==other.GetV1A()));
		}
};

template<class AJVertexDescriptor> class Merge : public CorrespondenceCategory {
private:
	AJVertexDescriptor m_V0A;
	AJVertexDescriptor m_V0B;
	AJVertexDescriptor m_V1;

public:
	Merge(){

	}

	void SetV0A(const AJVertexDescriptor & v0a){
		m_V0A=v0a;
	}
	void SetV0B(const AJVertexDescriptor & v0b){
		m_V0B=v0b;
	}

	void SetV1(const AJVertexDescriptor & v1){
		m_V1=v1;
	}


	AJVertexDescriptor GetV0A() const{
		return m_V0A;
	}
	AJVertexDescriptor GetV0B() const{
		return m_V0B;
	}
	AJVertexDescriptor GetV1() const{
		return m_V1;
	}

	bool operator==(const Merge & other) const{
		return m_V1==other.GetV1() && ( (m_V0A==other.GetV0A() && m_V0B==other.GetV0B()) || (m_V0A==other.GetV0B() && m_V0B==other.GetV0A()));
	}
};


template<class AJVertexDescriptor> class T1 : public CorrespondenceCategory{
private:
	AJVertexDescriptor m_V0A;
	AJVertexDescriptor m_V0B;
	AJVertexDescriptor m_V1A;
	AJVertexDescriptor m_V1B;

public:
	T1(){

	}
	void SetV0A(const AJVertexDescriptor & v0a){
		m_V0A=v0a;
	}
	void SetV0B(const AJVertexDescriptor & v0b){
		m_V0B=v0b;
	}
	void SetV1A(const AJVertexDescriptor & v1a){
		m_V1A=v1a;
	}
	void SetV1B(const AJVertexDescriptor & v1b){
		m_V1B=v1b;
	}

	AJVertexDescriptor GetV0A() const {
		return m_V0A;
	}
	AJVertexDescriptor GetV0B() const{
		return m_V0B;
	}
	AJVertexDescriptor GetV1A() const{
		return m_V1A;
	}
	AJVertexDescriptor GetV1B() const{
		return m_V1B;
	}

};

template<class AJVertexDescriptor> class T2 : public CorrespondenceCategory{

private:
	std::vector<AJVertexDescriptor> m_V0s;
	std::vector<AJVertexDescriptor> m_V1s;

public:
	typedef typename std::vector<AJVertexDescriptor>::iterator V0Iterator;
	typedef typename std::vector<AJVertexDescriptor>::iterator V1Iterator;

	T2(){

	}

	void AddV0(const AJVertexDescriptor & v0){
		m_V0s.push_back(v0);
	}
	void AddV1(const AJVertexDescriptor & v1){
		m_V1s.push_back(v1);
	}
	V0Iterator BeginV0(){
		return m_V0s.begin();
	}

	V0Iterator EndV0(){
		return m_V0s.end();
	}

	V0Iterator BeginV1(){
		return m_V1s.begin();
	}

	V0Iterator EndV1(){
		return m_V1s.end();
	}

	unsigned int GetNumberOfV0s(){
		return m_V0s.size();
	}

	unsigned int GetNumberOfV1s(){
		return m_V1s.size();
	}

};
#endif

template<class TTissue> class AJSubgraph{
public:

	//typedef boost::subgraph<TGraphType> SubgraphType;
	//typedef typename boost::graph_traits<SubgraphType>::vertex_descriptor VertexType;


	typedef typename TTissue::AJGraphType::AJEdgeHandler EdgeType;
	typedef typename TTissue::AJGraphType::AJVertexHandler VertexType;


	typedef std::set<VertexType> VertexSet;
	typedef std::set<EdgeType> EdgeSet;
	typedef typename VertexSet::iterator VertexIterator;
	typedef typename EdgeSet::iterator EdgeIterator;
	//typedef typename boost::graph_traits<TGraphType>::vertex_iterator VertexIterator;
	//typedef typename boost::graph_traits<TGraphType>::edge_iterator EdgeIterator;
private:
	VertexSet m_Vertices;
	EdgeSet m_Edges;
	unsigned int m_Order; //0 - null 1 - vertex 2 -edge 3-face

	//SubgraphType  m_Subgraph;
	//SubgraphType  m_Root;
public:
	AJSubgraph(){
		m_Order=0;
	}
#if 0

	AJSubgraph(const AJSubgraph & other) {
		//m_Subgraph(other.m_Subgraph){
		//m_Root=other.m_Root;
		m_Order=other.m_Order;
		m_Vertices=other.m_Vertices;
		m_Edges=other.m_Edges;

	}
	void operator=(const AJSubgraph & other){
		//m_Subgraph=other.m_Subgraph;
		m_Order=other.m_Order;
		m_Vertices=other.m_Vertices;
		m_Edges=other.m_Edges;

		//m_Root=other.m_Root;
	}
#endif
	bool Contains( const VertexType & vertex) const{
		//return boost::vertex(m_Subgraph.global_to_local(vertex))!=boost::graph_traits<SubgraphType>::null_vertex();
		return m_Vertices.find(vertex)!=m_Vertices.end();
	}
	void AddVertex(const VertexType & vertex){
		m_Vertices.insert(vertex);
		//boost::add_vertex(vertex,m_Subgraph);
	}
	void AddEdge(const EdgeType & edge){
		m_Edges.insert(edge);
	}
	unsigned int GetNumberOfVertices(){
		return m_Vertices.size();
	}
	unsigned int GetNumberOfEdges(){
		return m_Edges.size();
	}
	void SetOrder(int order){
		m_Order=order;
	}
	unsigned int GetOrder() const{
		return m_Order;
	}
	bool GetCardinality() const{
//		return boost::num_vertices(m_Subgraph);
		return m_Vertices.size();
	}

	VertexIterator BeginVertices() const{
		return m_Vertices.begin();
		//return boost::vertices(m_Subgraph).first;
	}

	VertexIterator EndVertices() const{
		return m_Vertices.end();
		//return boost::vertices(m_Subgraph).second;
	}
	EdgeIterator BeginEdges() const{
		return m_Edges.begin();
	}
	EdgeIterator EndEdges() const{
		return m_Edges.end();
	}
#if 0
	EdgeIterator BeginEdges(){
		return boost::edges(m_Subgraph).first;

	}
	EdgeIterator EndEdges(){
			return boost::edges(m_Subgraph).second;

	}
#endif
#if 1
	bool operator<(const AJSubgraph & other)const{
		return m_Vertices < other.m_Vertices;
	}
#endif
	bool operator==(const AJSubgraph & other)const{
#if 0
		std::set<typename boost::graph_traits<TGraphType>::vertex_descriptor> myVertices,otherVertices;
		std::list<typename boost::graph_traits<TGraphType>::vertex_descriptor> result;

		BGL_FORALL_VERTICES_T(v,m_Subgraph,TGraphType){
			myVertices.insert(m_Subgraph.local_to_global(v));
		}
		BGL_FORALL_VERTICES_T(v,other.m_Subgraph,TGraphType){
			otherVertices.insert(m_Subgraph.local_to_global(v));
		}

		std::set_symmetric_difference(myVertices.begin(),myVertices.end(),otherVertices.begin(),otherVertices.end(),std::back_inserter(result));
#endif
		//std::list<typename boost::graph_traits<TGraphType>::vertex_descriptor> result;
		//std::list<unsigned long> result;
		//std::set_symmetric_difference(m_Vertices.begin(),m_Vertices.end(),other.m_Vertices.begin(),other.m_Vertices.end(),std::back_inserter(result));

		//return result.size()==0;
		return m_Vertices==other.m_Vertices;
	}
};



template<class TAJSubgraph> class AJCorrespondence{
public:
	typedef TAJSubgraph AJSubgraphType;
	AJSubgraphType m_Antecessor;
	AJSubgraphType m_Successor;
	double m_Cost;

	AJCorrespondence(){
		m_Cost=std::numeric_limits<double>::max();
	}

	AJSubgraphType GetAntecessor() const{
		return m_Antecessor;
	}

	void SetAntecessor(const AJSubgraphType & antecessor){
		m_Antecessor=antecessor;
	}
	AJSubgraphType GetSuccessor() const{
			return m_Successor;
	}
	void SetSuccessor(const AJSubgraphType & successor){
		m_Successor=successor;
	}
	void SetCost(double cost){
		m_Cost=cost;
	}
	double GetCost() const{
		return m_Cost;
	}
	bool operator<(const AJCorrespondence & other) const{
		if(m_Antecessor<other.GetAntecessor()){
			return true;
		}else if(other.GetAntecessor()<m_Antecessor){
			return false;
		}else{
			if(m_Successor<other.GetSuccessor()){
				return true;
			}else{
				return false;
			}
		}
	}
};

template<class TTissue> class AJCorrespondenceSet {
public:


	typedef AJSubgraph<TTissue> AJSubgraphType;
	typedef AJCorrespondence<AJSubgraphType> AJCorrespondenceType;


	struct antecessor{};
	struct successor{};
	struct correspondence{};
	typedef multi_index_container<
			AJCorrespondenceType,
	    indexed_by<
	       ordered_unique<tag<correspondence>, composite_key<AJCorrespondenceType, member<AJCorrespondenceType, AJSubgraphType, &AJCorrespondenceType::m_Antecessor>,
	                                                                                   member<AJCorrespondenceType, AJSubgraphType, &AJCorrespondenceType::m_Successor> > >,
		   ordered_non_unique<tag<antecessor>, member<AJCorrespondenceType, AJSubgraphType, &AJCorrespondenceType::m_Antecessor> >,
		   ordered_non_unique<tag<successor>, member<AJCorrespondenceType, AJSubgraphType, &AJCorrespondenceType::m_Successor> >

	        >
	    > CorrespondenceContainer;

	CorrespondenceContainer m_Correspondences;

	typedef typename CorrespondenceContainer::template index<correspondence>::type CorrespondenceIndex;
	typedef typename CorrespondenceIndex::iterator CorrespondenceIterator;
	typedef std::pair<CorrespondenceIterator,CorrespondenceIterator> CorrespondenceResultSet;

	typedef typename CorrespondenceContainer::template index<antecessor>::type AntecessorIndex;
	typedef typename AntecessorIndex::iterator AntecessorIterator;
	typedef std::pair<AntecessorIterator,AntecessorIterator> AntecessorResultSet;

	typedef typename CorrespondenceContainer::template index<successor>::type SuccessorIndex;
	typedef typename SuccessorIndex::iterator SuccessorIterator;
	typedef std::pair<SuccessorIterator,SuccessorIterator> SuccessorResultSet;

	void AddCorrespondence(const AJSubgraphType & antecessor, const AJSubgraphType & successor,double cost){
		AJCorrespondenceType correspondence;
		correspondence.SetAntecessor(antecessor);
		correspondence.SetSuccessor(successor);
		correspondence.SetCost(cost);
		m_Correspondences.insert(correspondence);
		//m_Correspondences.push_back(correspondence);

	}

	CorrespondenceIterator GetCorrespondence(const AJSubgraphType & antecessor, const AJSubgraphType & successor){

		return m_Correspondences.template get<correspondence>().find(boost::make_tuple(antecessor,successor));

	}
	bool RemoveCorrespondence(const AJSubgraphType & antecessor, const AJSubgraphType & succesor){

		auto correspondenceIt=this->GetCorrespondence(antecessor,succesor);
		if(correspondenceIt!=m_Correspondences.end()){
			m_Correspondences.erase(correspondenceIt);
			return true;
		}else{
			return false;
		}

	}
	void Clear(){
		m_Correspondences.clear();
	}
	unsigned int GetNumberOfCorrespondences(){
		return m_Correspondences.size();
	}
	AntecessorResultSet FindByAntecessor(const  AJSubgraphType & ante) const{
		return m_Correspondences.template get<antecessor>().equal_range(ante);
	}

	SuccessorResultSet FindBySuccessor(const AJSubgraphType & succe) const{

		return m_Correspondences.template get<successor>().equal_range(succe);
	}
	CorrespondenceIterator BeginCorrespondences() const{
		return m_Correspondences.begin();
	}
	CorrespondenceIterator EndCorrespondences() const{
		return m_Correspondences.end();
	}


#if 0
public:
	typedef AJCorrespondenceSet<AJVertexDescriptor> Self;
	typedef itk::SmartPointer<Self> Pointer;
	typedef Succesor<AJVertexDescriptor> SuccesorType;
	typedef Split<AJVertexDescriptor> SplitType;
	typedef Merge<AJVertexDescriptor> MergeType;
	typedef T1<AJVertexDescriptor> T1Type;
	typedef T2<AJVertexDescriptor> T2Type;
	typedef Entering<AJVertexDescriptor> EnteringType;
	typedef Leaving<AJVertexDescriptor> LeavingType;

	typedef typename std::vector<LeavingType>::iterator LeavingIterator;
	typedef typename std::vector<EnteringType>::iterator EnteringIterator;
	typedef typename std::vector<SuccesorType>::iterator SuccesorIterator;
	typedef typename std::vector<SplitType>::iterator SplitIterator;
	typedef typename std::vector<MergeType>::iterator MergeIterator;
	typedef typename std::vector<T1Type>::iterator T1Iterator;
	typedef typename std::vector<T2Type>::iterator T2Iterator;

	itkNewMacro(Self);
private:
	std::vector<SuccesorType> m_Succesors;
	std::vector<SplitType> m_Splits;
	std::vector<MergeType> m_Merges;
	std::vector<T1Type> m_T1s;
	std::vector<T2Type> m_T2s;
	std::vector<EnteringType> m_Enterings;
	std::vector<LeavingType>  m_Leavings;
public:
	AJCorrespondenceSet(){

	}
	AJCorrespondenceSet(const AJCorrespondenceSet & other){
		for (auto it = other.BeginSuccesors();it!=other.EndSuccesors();it++){
			this->AddCorrespondence(*it);
		}
		for (auto it = other.BeginSplits();it!=other.EndSplits();it++){
			this->AddCorrespondence(*it);
		}
		for (auto it = other.BeginMerges();it!=other.EndMerges();it++){
			this->AddCorrespondence(*it);
		}
		for (auto it = other.BeginT1s();it!=other.EndT1s();it++){
			this->AddCorrespondence(*it);
		}
	}
	void operator=(const AJCorrespondenceSet & other){
		m_Succesors.clear();
		m_Splits.clear();
		m_Merges.clear();
		m_T1s.clear();
		for (auto it = other.BeginSuccesors();it!=other.EndSuccesors();it++){
				this->AddCorrespondence(*it);
			}
			for (auto it = other.BeginSplits();it!=other.EndSplits();it++){
				this->AddCorrespondence(*it);
			}
			for (auto it = other.BeginMerges();it!=other.EndMerges();it++){
				this->AddCorrespondence(*it);
			}
			for (auto it = other.BeginT1s();it!=other.EndT1s();it++){
				this->AddCorrespondence(*it);
			}
	}
	void AddCorrespondence( const SuccesorType & succesor){
		m_Succesors.push_back(succesor);
	}
	void AddCorrespondence( const SplitType & split){
		m_Splits.push_back(split);
	}

	void AddCorrespondence( const MergeType & merge){
		m_Merges.push_back(merge);
	}

	void AddCorrespondence( const EnteringType & entering){
		m_Enterings.push_back(entering);
	}

	void AddCorrespondence( const LeavingType & leaving){
		m_Leavings.push_back(leaving);
	}

	void AddCorrespondence( const T1Type & t1){
		m_T1s.push_back(t1);
	}

	void AddCorrespondence( const T2Type & t2){
		m_T2s.push_back(t2);
	}
	SuccesorType GetNSuccesor(int n){
		return m_Succesors[n];
	}

	void DeleteNSuccesor(int n){
		this->DeleteCorrespondence(m_Succesors[n]);
	}
	void DeleteCorrespondence( const SuccesorType & succesor){
		m_Succesors.erase(std::find(m_Succesors.begin(),m_Succesors.end(),succesor));
	}


	void DeleteNMerge(int n){
		this->DeleteCorrespondence(m_Merges[n]);
	}
	void DeleteCorrespondence( const MergeType & merge){
			m_Merges.erase(std::find(m_Merges.begin(),m_Merges.end(),merge));
	}
	void DeleteNSplit(int n){
		this->DeleteCorrespondence(m_Splits[n]);
	}
	void DeleteCorrespondence( const SplitType & split){
		m_Splits.erase(std::find(m_Splits.begin(),m_Splits.end(),split));
	}

	SuccesorIterator BeginSuccesors(){
		return m_Succesors.begin();
	}
	SuccesorIterator EndSuccesors(){
		return m_Succesors.end();
	}

	EnteringIterator BeginEnterings(){
		return m_Enterings.begin();
	}

	EnteringIterator EndEnterings(){
		return m_Enterings.end();
	}

	LeavingIterator BeginLeavings(){
		return m_Leavings.begin();
	}
	LeavingIterator EndLeavings(){
		return m_Leavings.end();
	}

	SplitIterator BeginSplits(){
		return m_Splits.begin();
	}

	SplitIterator EndSplits(){
		return m_Splits.end();
	}

	MergeIterator BeginMerges(){
		return m_Merges.begin();
	}

	MergeIterator EndMerges(){
		return m_Merges.end();
	}

	T1Iterator BeginT1s(){
		return m_T1s.begin();
	}

	T1Iterator EndT1s(){
		return m_T1s.end();
	}

	T2Iterator BeginT2s(){
		return m_T2s.begin();
	}

	T2Iterator EndT2s(){
		return m_T2s.end();
	}
	unsigned int GetNumberOfSuccesors(){
		return m_Succesors.size();
	}
	unsigned int GetNumberOfEnterings(){
		return m_Enterings.size();
	}
	unsigned int GetNumberOfLeavings(){
		return m_Leavings.size();
	}
	unsigned int GetNumberOfSplits(){
		return m_Splits.size();
	}
	unsigned int GetNumberOfMerges(){
		return m_Merges.size();
	}
	unsigned int GetNumberOfT1s(){
		return m_T1s.size();
	}
	unsigned int GetNumberOfT2s(){
		return m_T2s.size();
	}
	LeavingIterator FindLeaving(const AJVertexDescriptor & v0){
		return std::find_if(m_Leavings.begin(),m_Leavings.end(), [&] (const LeavingType & l) { return l.GetV0()==v0; } );
	}
	SuccesorIterator FindSuccesor(const AJVertexDescriptor & v0){
		return std::find_if(m_Succesors.begin(),m_Succesors.end(), [&] (const SuccesorType & s) { return s.GetV0()==v0; } );
	}

	SplitIterator FindSplit(const AJVertexDescriptor & v0){
		return std::find_if(m_Splits.begin(),m_Splits.end(), [&] (const SplitType & s) { return s.GetV0()==v0; } );
	}

	MergeIterator FindMerge(const AJVertexDescriptor & v0a, const AJVertexDescriptor & v0b){
		return std::find_if(m_Merges.begin(),m_Merges.end(), [&] (const MergeType & m) { return (m.GetV0A()==v0a && m.GetV0B()==v0b) || (m.GetV0A()==v0b && m.GetV0B()==v0a) ; } );
	}

	T1Iterator FindT1(const AJVertexDescriptor & v0a, const AJVertexDescriptor & v0b){
		return std::find_if(m_T1s.begin(),m_T1s.end(), [&] (const T1Type & t1) { return (t1.GetV0A()==v0a && t1.GetV0B()==v0b) || (t1.GetV0A()==v0b && t1.GetV0B()==v0a) ; } );
	}

	template<class TIterator> T2Iterator FindT2(const TIterator & begin,const TIterator & end ){
		auto it = m_T2s.begin();

		while(it!=m_T2s.end()){
			std::vector<AJVertexDescriptor> difference;
			std::set_symmetric_difference(begin,end,it->BeginV0(),it->EndV0(),std::back_inserter(difference));
			if(difference.size()==0){
				return it;
			}
			++it;
		}
		return it;
	}
#endif
};
#endif /* AJCORRESPONDENCESET_H_ */
