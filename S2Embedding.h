/*
 * S2Embedding.h
 *
 *  Created on: Jul 2, 2015
 *      Author: morgan
 */

#ifndef S2EMBEDDING_H_
#define S2EMBEDDING_H_


double distAng(double lat0,double lon0, double lat1, double lon1){
	double a = pow(std::sin((lat1-lat0)/2),2)+std::cos(lat1)*std::cos(lat0)*std::pow(std::sin((lon1-lon0)/2),2);
	return 2*atan2(std::sqrt(a),std::sqrt(1-a));
}


vnl_matrix<double> S2Embedding(vnl_matrix<double> & D,vnl_matrix<double> & sphr){

	//std::cout << D << std::endl;
	int NumberOfVertices=D.rows();


	//std::cout << svd.pinverse(2) << std::endl;

	//vnl_matrix<double> sphr(D.rows(),2);


	vnl_vector<double> alpha(D.rows());
	int nmaxreloc=50;
	int niter=0;
	bool stopCondition=false;
	while(!stopCondition){
		niter=niter+1;
		for(unsigned int v=0;v<NumberOfVertices;v++){
			double lat0 = sphr(v,0);
			double lon0 = sphr(v,1);


			auto columnDv=D.get_column(v);
			std::vector<unsigned> neighs;


			for(int c=0;c<columnDv.size();c++){
				if(columnDv[c]>0){
					neighs.push_back(c);
				}
			}
			assert(neighs.size()>0);
			//std::cout << neighs.size() << std::endl;
			vnl_vector<double> neighDistances(neighs.size());
			{
				int c=0;
				for(auto neigh : neighs){
					neighDistances[c]=D(neigh,v);
					c++;
				}
			}
			for(int j=0;j<nmaxreloc;j++){
				double lat2=sphr(v,0);
				double lon2=sphr(v,1);

				vnl_vector<double> lat1(neighs.size());//= sphr.get_column(0);
				vnl_vector<double> lon1(neighs.size());//= sphr.get_column(1);

				int c=0;
				for(auto neigh: neighs){
					lat1[c]=sphr(neigh,0);
					lon1[c]=sphr(neigh,1);
					c++;
				}



				auto tmpY = std::cos(lat2)*(lon2-lon1).apply(sin);
				auto tmpX = lat1.apply(cos)*std::sin(lat2) - element_product(lat1.apply(sin)*std::cos(lat2),(lon2-lon1).apply(cos));

				auto az = tmpX;
				std::transform(tmpY.begin(),tmpY.end(),tmpX.begin(),az.begin(),::atan2);

//				std::cout << az << std::endl;

				//std::cout << "Az" << std::endl;
				//std::cout << az << std::endl;
				if(lat2>= M_PI/2){
					az.fill(0);
				}
				if(lat2<= -M_PI/2){
					az.fill(M_PI);
				}
				for(int k=0;k<lat1.size();k++){
					if(lat1[k]>=M_PI/2){
						az.fill(M_PI);
					}
					if(lat1[k]<= -M_PI/2){
						az.fill(0);
					}
				}

				{
					auto az2=az;
					for(int k=0;k<lat1.size();k++){
						if(lat1[k]>=M_PI/2){
							az2[k]= M_PI;
						}
						if(lat1[k]<=-M_PI/2){
							az2[k]=0;
						}
					}

					//std::cout << neighDistances << std::endl;
					auto latP= (element_product(lat1.apply(sin),neighDistances.apply(cos)) +element_product(lat1.apply(cos),element_product(neighDistances.apply(sin),az2.apply(cos)))).apply(asin);

					auto tmpY= element_product(neighDistances.apply(sin),az2.apply(sin));
					auto tmpX= element_product(lat1.apply(cos),neighDistances.apply(cos)) - element_product(element_product(lat1.apply(sin),neighDistances.apply(sin)),az2.apply(cos));
					auto lonP= tmpX;

					std::transform(tmpY.begin(),tmpY.end(),tmpX.begin(),lonP.begin(),::atan2);
					lonP=lon1+lonP;


					//std::cout << lat2 << std::endl;
					//std::cout << lon2 << std::endl;

					auto x = element_product(latP.apply(cos),lonP.apply(cos));
					auto y = element_product(latP.apply(cos),lonP.apply(sin));
					auto z = latP.apply(sin);

					auto lonI = std::atan2(y.sum(),x.sum());
					auto latI = std::atan2(z.sum(),std::sqrt(std::pow(x.sum(),2)+ std::pow(y.sum(),2)));

					//std::cout << j << " Improvement " << std::sqrt(pow(latI -sphr(v,0),2) + pow(lonI -sphr(v,1),2)) << std::endl;
//					std::cout << lonI << " " << latI << std::endl;
					sphr(v,0)= latI;
					sphr(v,1)= lonI;

					double dist =distAng(lat2, lon2, latI, lonI);
		            //if ( dist< 0.0873){
					if ( dist< 0.000000001){
		            	break;
		            }

				}

			}

			alpha[v]=distAng(sphr(v,0),sphr(v,1),lat0,lon0);

		}
		std::cout << niter << " " << alpha.max_value() << std::endl;
		if(alpha.max_value()<1e-12 || niter>20){
			stopCondition=true;
		}
	}
	return sphr;
}
vnl_matrix<double> S2Embedding(vnl_matrix<double> & D){
	vnl_svd<double > svd(D);
	vnl_matrix<double> sphr = svd.U().get_n_columns(0,2);
	return S2Embedding(D,sphr);
}

#endif /* S2EMBEDDING_H_ */
