#include "Math.hpp"

arma::mat33 build_psi_tht_phi_TM(const double &psi, const double &tht, const double &phi) {
    double spsi = sin(psi);
    double cpsi = cos(psi);
    double stht = sin(tht);
    double ctht = cos(tht);
    double sphi = sin(phi);
    double cphi = cos(phi);

    arma::mat33 AMAT;
    AMAT(0, 0) = cpsi * ctht;
    AMAT(1, 0) = cpsi * stht * sphi - spsi * cphi;
    AMAT(2, 0) = cpsi * stht * cphi + spsi * sphi;
    AMAT(0, 1) = spsi * ctht;
    AMAT(1, 1) = spsi * stht * sphi + cpsi * cphi;
    AMAT(2, 1) = spsi * stht * cphi - cpsi * sphi;
    AMAT(0, 2) = -stht;
    AMAT(1, 2) = ctht * sphi;
    AMAT(2, 2) = ctht * cphi;

    return AMAT;
}

arma::mat33 skew_sym(arma::vec3 vec) {
    arma::mat33 RESULT;

    RESULT(0, 0) = 0;
    RESULT(1, 0) = vec(2);
    RESULT(2, 0) = -vec(1);
    RESULT(0, 1) = -vec(2);
    RESULT(1, 1) = 0;
    RESULT(2, 1) = vec(0);
    RESULT(0, 2) = vec(1);
    RESULT(1, 2) = -vec(0);
    RESULT(2, 2) = 0;

    return RESULT;
}