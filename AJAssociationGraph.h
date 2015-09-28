/*
 * AssociationGraph.h
 *
 *  Created on: May 20, 2014
 *      Author: morgan
 */

#ifndef ASSOCIATIONGRAPH_H_
#define ASSOCIATIONGRAPH_H_
#include <vnl/vnl_sparse_matrix.h>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <tuple>
namespace ttt {

template<class TObservation> class AssociationGraph{

public:

	typedef typename AJCorrespondenceSet<TObservation>::EnteringType EnteringType;
	typedef typename AJCorrespondenceSet<TObservation>::LeavingType  LeavingType;
	typedef typename AJCorrespondenceSet<TObservation>::SuccesorType SuccesorType;
	typedef typename AJCorrespondenceSet<TObservation>::SplitType    SplitType;
	typedef typename AJCorrespondenceSet<TObservation>::MergeType    MergeType;
	typedef typename AJCorrespondenceSet<TObservation>::T1Type 	  T1Type;
	typedef typename AJCorrespondenceSet<TObservation>::T2Type 	  T2Type;

public:
	template<class THypothesis> class Hypothesis : public THypothesis{
	private:
		double m_Cost;
	public:
		void SetCost(double cost){
			m_Cost=cost;
		}
		double GetCost() const{
			return m_Cost;
		}
		bool operator<(const Hypothesis<THypothesis> & other) const{
			return m_Cost < other.GetCost();
		}
	};
	typedef Hypothesis<EnteringType> EnteringHypothesisType;
	typedef Hypothesis<LeavingType> LeavingHypothesisType;
	typedef Hypothesis<SuccesorType> SuccesorHypothesisType;
	typedef Hypothesis<SplitType> SplitHypothesisType;
	typedef Hypothesis<MergeType> MergeHypothesisType;
	typedef Hypothesis<T1Type> T1HypothesisType;
	typedef Hypothesis<T2Type> T2HypothesisType;

private:
	std::vector<EnteringHypothesisType > m_Enterings;
	std::vector<LeavingHypothesisType > m_Leavings;
	std::vector<SuccesorHypothesisType > m_Succesors;
	std::vector<SplitHypothesisType> m_Splits;
	std::vector<MergeHypothesisType > m_Merges;
	std::vector<T1HypothesisType > m_T1s;
	std::vector<T2HypothesisType> m_T2s;


	std::map<int,EnteringType> m_EncodedEntering;
	std::map<int,LeavingType> m_EncodedLeaving;
	std::map<int,SuccesorType> m_EncodedSuccesor;
	std::map<int,SplitType> m_EncodedSplit;
	std::map<int,MergeType> m_EncodedMerge;
	std::map<int,T1Type> m_EncodedT1;
	std::map<int,T2Type> m_EncodedT2;

	int m_NumberOfLeftNodes;
	int m_NumberOfRightNodes;
	int m_NumberOfSuccesorHypothesis;
	int m_NumberOfSplitHypothesis;
	int m_NumberOfMergeHypothesis;
	int m_NumberOfT1Hypothesis;
	int m_NumberOfT2Hypothesis;
	int m_NumberOfLeavingHypothesis;
	int m_NumberOfEnteringHypothesis;


public:

	typedef AJCorrespondenceSet<TObservation> AJCorrespondenceType;


public:
	AssociationGraph(){
		m_NumberOfRightNodes=0;
		m_NumberOfLeftNodes=0;
		m_NumberOfSuccesorHypothesis=0;
		m_NumberOfEnteringHypothesis=0;
		m_NumberOfLeavingHypothesis=0;
		m_NumberOfSplitHypothesis=0;
		m_NumberOfMergeHypothesis=0;
		m_NumberOfT1Hypothesis=0;
		m_NumberOfT2Hypothesis=0;


	}

	void AddLeftNode(const TObservation & left){
		this->m_NumberOfLeftNodes++;
	}

	void AddRightNode(const TObservation & right){
		this->m_NumberOfRightNodes++;
	}

	void AddEnteringHypothesis(const TObservation & right,double cost){
		Hypothesis<EnteringType> entering;
		entering.SetCost(cost);
		entering.SetV1(right);
		this->m_Enterings.push_back(entering);
		this->m_NumberOfEnteringHypothesis++;
	}

	void AddLeavingHypothesis(const TObservation & left,double cost){
		Hypothesis<LeavingType> leaving;
		leaving.SetCost(cost);
		leaving.SetV0(left);
		this->m_Leavings.push_back(leaving);
		this->m_NumberOfEnteringHypothesis++;
	}

	void AddSuccesorHypothesis(const SuccesorHypothesisType & succesor){
		this->m_Succesors.push_back(succesor);
		this->m_NumberOfSuccesorHypothesis++;

	}

	void AddSplitHypothesis(const SplitHypothesisType & split){

		this->m_Splits.push_back(split);
		this->m_NumberOfSplitHypothesis++;

	}

	void AddMergeHypothesis(const MergeHypothesisType & merge){
		this->m_Merges.push_back(merge);
		this->m_NumberOfMergeHypothesis++;
	}

	void AddT1Hypothesis(const T1HypothesisType & t1){

		this->m_T1s.push_back(t1);
		this->m_NumberOfT1Hypothesis++;
	}

	void AddT2Hypothesis(const T2HypothesisType & t2){

		this->m_T2s.push_back(t2);

		this->m_NumberOfT2Hypothesis++;

	}


	void Prune(){
		std::cout << "Before Prunning" << std::endl;
		this->Print();

		std::sort(m_T1s.begin(),m_T1s.end());
		std::sort(m_T2s.begin(),m_T2s.end());
		std::sort(m_Splits.begin(),m_Splits.end());
		std::sort(m_Merges.begin(),m_Merges.end());

		//std::cout << m_Merges[0].GetCost() << " " << m_Merges[m_Merges.size()-1].GetCost() << std::endl;

		std::vector<Hypothesis<T1Type> > newT1(m_T1s.begin(),m_T1s.begin()+m_T1s.size());
		m_T1s=newT1;

		std::vector<Hypothesis<T2Type> > newT2(m_T2s.begin(),m_T2s.begin()+m_T2s.size());
		m_T2s=newT2;

		std::vector<Hypothesis<SplitType> > newSplit(m_Splits.begin(),m_Splits.begin()+ m_Splits.size());//0.01*m_Splits.size());
		m_Splits=newSplit;


		std::vector<Hypothesis<MergeType> > newMerge(m_Merges.begin(),m_Merges.begin()+ m_Merges.size());//0.01*m_Merges.size());
		m_Merges=newMerge;

		std::vector<Hypothesis<SuccesorType> > newSuccesors(m_Succesors.begin(),m_Succesors.begin()+m_Succesors.size());
		m_Succesors=newSuccesors;

		std::cout << "After Prunning" << std::endl;
		this->Print();

		//std::cout << m_Merges[0].GetCost() << " " << m_Merges[m_Merges.size()-1].GetCost() << std::endl;



	}

	void Print(){

		std::cout << "Number of T1s" << m_T1s.size() << std::endl;
		std::cout << "Number of T2s" << m_T2s.size() << std::endl;
		std::cout << "Number of Enterings" << m_Enterings.size() << std::endl;
		std::cout << "Number of Leavings" << m_Leavings.size() << std::endl;
		std::cout << "Number of Splits" << m_Splits.size() << std::endl;
		std::cout << "Number of Merges" << m_Merges.size() << std::endl;
		std::cout << "Number of Succesors" << m_Succesors.size() << std::endl;

	}
	void GetProblem(vnl_vector<double> & costVector, vnl_vector<double> & flowVector, vnl_vector<double> & capacityVector, vnl_sparse_matrix<double> & flowMatrix){

		int nRows=1 + m_NumberOfLeftNodes +1+ m_NumberOfRightNodes + 1  +1;
		int nCols=(m_NumberOfRightNodes+1) + m_NumberOfSuccesorHypothesis + m_NumberOfSplitHypothesis + m_NumberOfMergeHypothesis + m_NumberOfLeavingHypothesis +  m_NumberOfEnteringHypothesis + m_NumberOfT1Hypothesis + m_NumberOfT2Hypothesis + (m_NumberOfLeftNodes +1) +1; //AD-HYPOTHESIS


		costVector.clear();
		flowVector.clear();
		capacityVector.clear();
		flowMatrix.clear();
		flowMatrix.resize(nRows,nCols);
		assert(flowVector.set_size(nRows));
		assert(costVector.set_size(nCols));
		assert(capacityVector.set_size(nCols));
		int edge=0;

		//TP-A Edge
		int tpVertex=0;
		int aVertex=1;

		flowMatrix.put(tpVertex,edge,-1);
		flowMatrix.put(aVertex,edge,1);

		costVector.put(edge,0);
		capacityVector.put(edge,this->m_NumberOfRightNodes);

		edge++;

		//TP-L Edges
		int baseLeftVertex=2;
		for(int i=0;i<this->m_NumberOfLeftNodes;i++){
			flowMatrix.put(tpVertex,edge,-1);
			flowMatrix.put(baseLeftVertex+i,edge,1);
			costVector.put(edge,0);
			capacityVector.put(edge,1);
			edge++;
		}

		int baseRightVertex=baseLeftVertex+this->m_NumberOfLeftNodes;

		int dVertex = baseRightVertex+ this->m_NumberOfRightNodes;

		//A-D Edge
		flowMatrix.put(aVertex,edge,-1);
		flowMatrix.put(dVertex,edge,1);

		costVector.put(edge,0);
		capacityVector.put(edge,this->m_NumberOfLeftNodes);
		edge++;

		int tnVertex = dVertex+1;

		//R-Tn Edges
		for(int i=0;i<this->m_NumberOfRightNodes;i++){
			flowMatrix.put(baseRightVertex+i,edge,-1);
			flowMatrix.put(tnVertex,edge,1);
			costVector.put(edge,0);
			capacityVector.put(edge,1);
			edge++;
		}
		//D-Tn Edge
		flowMatrix.put(dVertex,edge,-1);
		flowMatrix.put(tnVertex,edge,1);
		costVector.put(edge,0);
		capacityVector.put(edge,this->m_NumberOfLeftNodes);
		edge++;

		for(auto entering : this->m_Enterings){
			flowMatrix.put(aVertex,edge,-1);
			flowMatrix.put(baseRightVertex+entering.GetV1(),edge,1);
			costVector.put(edge,entering.GetCost());
			capacityVector.put(edge,1);
			m_EncodedEntering[edge]=entering;
			edge++;

		}

		for(auto succesor : this->m_Succesors){
			flowMatrix.put(baseLeftVertex+succesor.GetV0(),edge,-1);
			flowMatrix.put(baseRightVertex+succesor.GetV1(),edge,1);
			costVector.put(edge,succesor.GetCost());
			capacityVector.put(edge,1);
			m_EncodedSuccesor[edge]=succesor;
			edge++;
		}

		for(auto leaving : this->m_Leavings){
			flowMatrix.put(baseLeftVertex+leaving.GetV0(),edge,-1);
			flowMatrix.put(dVertex,edge,1);
			costVector.put(edge,leaving.GetCost());
			capacityVector.put(edge,1);
			m_EncodedLeaving[edge]=leaving;
			edge++;
		}

		for(auto split : this->m_Splits){
			flowMatrix.put(baseLeftVertex+split.GetV0(),edge,-1);
			flowMatrix.put(aVertex,edge,-1);
			flowMatrix.put(baseRightVertex + split.GetV1A(),edge,1);
			flowMatrix.put(baseRightVertex + split.GetV1B(),edge,1);
			costVector.put(edge,split.GetCost());
			capacityVector.put(edge,1);
			m_EncodedSplit[edge]=split;
			edge++;
		}
		for(auto merge : this->m_Merges){
			flowMatrix.put(baseLeftVertex + merge.GetV0A(),edge,-1);
			flowMatrix.put(baseLeftVertex + merge.GetV0B(),edge,-1);

			flowMatrix.put(baseRightVertex+merge.GetV1(),edge,1);
			flowMatrix.put(dVertex,edge,1);

			costVector.put(edge,merge.GetCost());
			capacityVector.put(edge,1);
			m_EncodedMerge[edge]=merge;
			edge++;
		}
		for(auto t1 : this->m_T1s){
			flowMatrix.put(baseLeftVertex + t1.GetV0A(),edge,-1);
			flowMatrix.put(baseLeftVertex + t1.GetV0B(),edge,-1);
			flowMatrix.put(baseRightVertex + t1.GetV1A(),edge,1);
			flowMatrix.put(baseRightVertex + t1.GetV1B(),edge,1);

			costVector.put(edge,t1.GetCost());
			capacityVector.put(edge,1);
			m_EncodedT1[edge]=t1;
			edge++;
		}

		for(auto t2 : this->m_T2s){
			for(auto t2It = t2.BeginV0() ;t2It!=t2.EndV0();t2It++){
				flowMatrix.put(baseLeftVertex + *t2It,edge,-1);
			}
			for(auto t2It = t2.BeginV1() ;t2It!=t2.EndV1();t2It++){
				flowMatrix.put(baseRightVertex + *t2It,edge,+1);

			}
			flowMatrix.put(dVertex,edge,t2.GetNumberOfV1s()-t2.GetNumberOfV0s());
			costVector.put(edge,t2.GetCost());
			capacityVector.put(edge,1);
			m_EncodedT2[edge]=t2;
			edge++;
		}

		flowVector.fill(0);


		flowVector[tpVertex]=-(m_NumberOfRightNodes+m_NumberOfLeftNodes);
		flowVector[tnVertex]=(m_NumberOfRightNodes+m_NumberOfLeftNodes);
	}

	void DecodeSolution(
			const vnl_vector<double> & solution,
			typename AJCorrespondenceSet<TObservation>::Pointer & correspondences){


		int flowSent=0;
		for(int c=0;c<solution.size();c++){
			if(solution.get(c)>0){
				flowSent++;
				if(m_EncodedSuccesor.find(c)!=m_EncodedSuccesor.end()){
					correspondences->AddCorrespondence(m_EncodedSuccesor[c]);
				}else if(m_EncodedEntering.find(c)!=m_EncodedEntering.end()){
					correspondences->AddCorrespondence(m_EncodedEntering[c]);
				}else if(m_EncodedLeaving.find(c)!=m_EncodedLeaving.end()){
					correspondences->AddCorrespondence(m_EncodedLeaving[c]);
				}else if(m_EncodedSplit.find(c)!=m_EncodedSplit.end()){
					correspondences->AddCorrespondence(m_EncodedSplit[c]);
				}else if(m_EncodedMerge.find(c)!=m_EncodedMerge.end()){
					correspondences->AddCorrespondence(m_EncodedMerge[c]);
				}else if(m_EncodedT1.find(c)!=m_EncodedT1.end()){
					correspondences->AddCorrespondence(m_EncodedT1[c]);
				}else if(m_EncodedT2.find(c)!=m_EncodedT2.end()){
					correspondences->AddCorrespondence(m_EncodedT2[c]);
				}

			}
		}

	}


};
}
#endif /* ASSOCIATIONGRAPH_H_ */
