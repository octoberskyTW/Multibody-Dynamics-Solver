#ifndef MATH_HPP
#define MATH_HPP

#include <armadillo>

void build_psi_tht_phi_TM(const double &psi, const double &tht, const double &phi, arma::mat &AMAT);
arma::mat skew_sym(arma::vec3 const &vec);
void Matrix2Quaternion(arma::mat33 Matrix_in, arma::vec &Quaternion);
void Quaternion2Matrix(arma::vec4 const &Quaternion_in, arma::mat &Matrix_out);
arma::vec3 euler_angle(arma::mat33 TBD_in);
int sign(const double &variable);

#endif
