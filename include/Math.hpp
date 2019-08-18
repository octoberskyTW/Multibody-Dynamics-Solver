#ifndef MATH_HPP
#define MATH_HPP

#include <armadillo>

arma::mat33 build_psi_tht_phi_TM(const double &psi, const double &tht, const double &phi);
arma::mat33 skew_sym(arma::vec3 vec);

#endif
