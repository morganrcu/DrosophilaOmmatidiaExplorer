/*
 * distancePointToLine.h
 *
 *  Created on: Oct 20, 2015
 *      Author: morgan
 */

#ifndef DISTANCEPOINTTOLINE_H_
#define DISTANCEPOINTTOLINE_H_


 template<class TPoint> double distancePointToLine(const TPoint & x,const TPoint & a, const TPoint & b){


	auto ax = x - a;
	double normAX = ax.GetNorm();
	if (normAX == 0)
		return 0;
	auto ab = b - a;
	double r = ab * ax;

	r /= ax.GetNorm();

	double dist;
	if (r < 0) {
		dist = ax.GetNorm();
	} else if (r > 1) {
		itk::Vector<double, 3> xb = b - x;
		dist = xb.GetNorm();
	} else {
		dist = sqrt(fabs(pow(ax.GetNorm(), 2) - r * pow(ab.GetNorm(), 2)));
	}
	return dist;
}

#endif /* DISTANCEPOINTTOLINE_H_ */
