/*
 * CellEdge.h
 *
 *  Created on: Jun 19, 2015
 *      Author: morgan
 */

#ifndef CELLEDGE_H_
#define CELLEDGE_H_

template<class TAJEdge> class CellEdge{

public:
	CellEdge(){

	}
	void SetAJEdge(const TAJEdge & edge){
		m_AJEdge=edge;
	}
	TAJEdge GetAJEdge(){
		return m_AJEdge;
	}
private:
	TAJEdge m_AJEdge;

};


#endif /* CELLEDGE_H_ */
