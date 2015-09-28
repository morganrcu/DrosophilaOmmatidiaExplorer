/*
 * S2LLEEmbedding.h
 *
 *  Created on: Jul 6, 2015
 *      Author: morgan
 */

#ifndef S2LLEEMBEDDING_H_
#define S2LLEEMBEDDING_H_


#include <Eigen/Eigenvalues>

Eigen::MatrixXd S2LLEEmbedding( const Eigen::MatrixXd & W){

	std::cout << "W" << std::endl;
	std::cout <<  std::endl;
	Eigen::MatrixXd I(W.rows(),W.cols());
	I.setIdentity();
	Eigen::MatrixXd M=I-W;

	M = M.transpose()*M;


	Eigen::MatrixXd B(M.rows(),M.cols());
	Eigen::MatrixXd Bsqrt(B.rows(),B.cols());
	B.setIdentity();

	int it=0;
	int MaxIt=100;
	Eigen::MatrixXd Y(W.rows(),3);
	std::cout << "Norm " << B.norm() << std::endl;
	B=B/B.norm();
	while(it<MaxIt){
		Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXd> eigs;

		eigs.compute(M,B);

		//std::cout << eigs.eigenvalues().transpose()<< std::endl;
		Y= eigs.eigenvectors().block(0,1,M.rows(),3);




		for(int r=0;r<B.rows();r++){
			B(r,r)=1.0/(Y.row(r)*Y.row(r).transpose());
			Bsqrt(r,r)=sqrt(B(r,r));
			std::cout << B(r,r) << " ";
		}
		std::cout << std::endl;
		B=B/B.norm();


		it++;

	}

	//std::cout << Y << std::endl;
	Eigen::MatrixXd Z = Bsqrt*Y;
	for(int r=0;r<Y.rows();r++){
		for(int c=0;c<Y.cols();c++){
			std::cout << Z(r,c) << "\t";
		}
		std::cout << std::endl;

	}
	std::cout << std::endl;
	return Z;

}



#endif /* S2LLEEMBEDDING_H_ */
