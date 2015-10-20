/*
 * distancePointToLine.h
 *
 *  Created on: Oct 20, 2015
 *      Author: morgan
 */

#ifndef DISTANCEPOINTTOLINE_H_
#define DISTANCEPOINTTOLINE_H_

 template<class TPoint> double distancePointToLine(const TPoint & x,const TPoint & a, const TPoint & b){

	auto ba = b - a;
	auto xa = x - a;
	auto xb = x - b;

	double c1 = ba*xa;

	if(c1 <=0){
		return xa.GetNorm();
	}

	double c2 = ba*ba;

	if(c2<=c1){
		return xb.GetNorm();
	}

	double w = c1/c2;
	TPoint Pb = a + w*ba;
	return (x-Pb).GetNorm();
}

#endif /* DISTANCEPOINTTOLINE_H_ */
