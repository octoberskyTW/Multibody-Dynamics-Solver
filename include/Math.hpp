#ifndef MATH_HPP
#define MATH_HPP

#include <armadillo>

arma::mat33 build_psi_tht_phi_TM(const double &psi, const double &tht, const double &phi);
arma::mat33 skew_sym(arma::vec3 vec);
arma::vec4 Matrix2Quaternion(arma::mat33 Matrix_in);
arma::mat33 Quaternion2Matrix(arma::vec4 Quaternion_in);
arma::vec3 euler_angle(arma::mat33 TBD_in);
int sign(const double &variable);

#endif
