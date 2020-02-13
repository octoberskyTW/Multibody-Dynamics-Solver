#include "Math.hpp"
#include <algorithm>

#define PI 3.1415926536  ///< circumference of unit diameter circle
#define EPS 1.e-10      ///< machine precision error (type double)

void build_psi_tht_phi_TM(const double &psi, const double &tht, const double &phi, arma::mat &AMAT) {
    double spsi = sin(psi);
    double cpsi = cos(psi);
    double stht = sin(tht);
    double ctht = cos(tht);
    double sphi = sin(phi);
    double cphi = cos(phi);

    AMAT(0, 0) = cpsi * ctht;
    AMAT(1, 0) = cpsi * stht * sphi - spsi * cphi;
    AMAT(2, 0) = cpsi * stht * cphi + spsi * sphi;
    AMAT(0, 1) = spsi * ctht;
    AMAT(1, 1) = spsi * stht * sphi + cpsi * cphi;
    AMAT(2, 1) = spsi * stht * cphi - cpsi * sphi;
    AMAT(0, 2) = -stht;
    AMAT(1, 2) = ctht * sphi;
    AMAT(2, 2) = ctht * cphi;

    return;
}

arma::mat skew_sym(arma::vec3 const &vec) {
    arma::mat33 RESULT;

    RESULT(0, 0) = 0.0;
    RESULT(1, 0) = vec(2);
    RESULT(2, 0) = -vec(1);
    RESULT(0, 1) = -vec(2);
    RESULT(1, 1) = 0.0;
    RESULT(2, 1) = vec(0);
    RESULT(0, 2) = vec(1);
    RESULT(1, 2) = -vec(0);
    RESULT(2, 2) = 0.0;
    return RESULT;
}

void Matrix2Quaternion(arma::mat33 Matrix_in, arma::vec &Quaternion) {
    arma::vec4 q_square;
    double q_square_max;
    int j;
    Matrix_in = trans(Matrix_in);
    q_square[0] = fabs(1.0 + Matrix_in(0, 0) + Matrix_in(1, 1) + Matrix_in(2, 2));
    q_square[1] = fabs(1.0 + Matrix_in(0, 0) - Matrix_in(1, 1) - Matrix_in(2, 2));
    q_square[2] = fabs(1.0 - Matrix_in(0, 0) + Matrix_in(1, 1) - Matrix_in(2, 2));
    q_square[3] = fabs(1.0 - Matrix_in(0, 0) - Matrix_in(1, 1) + Matrix_in(2, 2));

    q_square_max = q_square.max();
    j = q_square.index_max();

    switch (j) {
    case 0:
        Quaternion(0) = 0.5 * sqrt(q_square_max);
        Quaternion(1) = 0.25 * (Matrix_in(2, 1) - Matrix_in(1, 2)) / Quaternion[0];
        Quaternion(2) = 0.25 * (Matrix_in(0, 2) - Matrix_in(2, 0)) / Quaternion[0];
        Quaternion(3) = 0.25 * (Matrix_in(1, 0) - Matrix_in(0, 1)) / Quaternion[0];
        break;
    case 1:
        Quaternion(1) = 0.5 * sqrt(q_square_max);
        Quaternion(0) = 0.25 * (Matrix_in(2, 1) - Matrix_in(1, 2)) / Quaternion[1];
        Quaternion(2) = 0.25 * (Matrix_in(1, 0) + Matrix_in(0, 1)) / Quaternion[1];
        Quaternion(3) = 0.25 * (Matrix_in(0, 2) + Matrix_in(2, 0)) / Quaternion[1];
        break;
    case 2:
        Quaternion(2) = 0.5 * sqrt(q_square_max);
        Quaternion(0) = 0.25 * (Matrix_in(0, 2) - Matrix_in(2, 0)) / Quaternion[2];
        Quaternion(1) = 0.25 * (Matrix_in(1, 0) + Matrix_in(0, 1)) / Quaternion[2];
        Quaternion(3) = 0.25 * (Matrix_in(2, 1) + Matrix_in(1, 2)) / Quaternion[2];
        break;
    case 3:
        Quaternion(3) = 0.5 * sqrt(q_square_max);
        Quaternion(0) = 0.25 * (Matrix_in(1, 0) - Matrix_in(0, 1)) / Quaternion[3];
        Quaternion(1) = 0.25 * (Matrix_in(2, 0) + Matrix_in(0, 2)) / Quaternion[3];
        Quaternion(2) = 0.25 * (Matrix_in(2, 1) + Matrix_in(1, 2)) / Quaternion[3];
        break;
    }

    return ;
}

void Quaternion2Matrix(arma::vec4 const &Quaternion_in, arma::mat &Matrix_out) {
    Matrix_out(0, 0) = 2. * (Quaternion_in(0) * Quaternion_in(0) + Quaternion_in(1) * Quaternion_in(1)) - 1.;
    Matrix_out(0, 1) = 2. * (Quaternion_in(1) * Quaternion_in(2) + Quaternion_in(0) * Quaternion_in(3));
    Matrix_out(0, 2) = 2. * (Quaternion_in(1) * Quaternion_in(3) - Quaternion_in(0) * Quaternion_in(2));
    Matrix_out(1, 0) = 2. * (Quaternion_in(1) * Quaternion_in(2) - Quaternion_in(0) * Quaternion_in(3));
    Matrix_out(1, 1) = 2. * (Quaternion_in(0) * Quaternion_in(0) + Quaternion_in(2) * Quaternion_in(2)) - 1.;
    Matrix_out(1, 2) = 2. * (Quaternion_in(2) * Quaternion_in(3) + Quaternion_in(0) * Quaternion_in(1));
    Matrix_out(2, 0) = 2. * (Quaternion_in(1) * Quaternion_in(3) + Quaternion_in(0) * Quaternion_in(2));
    Matrix_out(2, 1) = 2. * (Quaternion_in(2) * Quaternion_in(3) - Quaternion_in(0) * Quaternion_in(1));
    Matrix_out(2, 2) = 2. * (Quaternion_in(0) * Quaternion_in(0) + Quaternion_in(3) * Quaternion_in(3)) - 1.;
}

arma::vec3 euler_angle(arma::mat33 TBD_in)
{
    double psibdc(0), thtbdc(0), phibdc(0);
    double cthtbd(0);

    double mroll = 0;

    double tbd13 = TBD_in(0, 2);
    double tbd11 = TBD_in(0, 0);
    double tbd33 = TBD_in(2, 2);
    double tbd12 = TBD_in(0, 1);
    double tbd23 = TBD_in(1, 2);

    arma::vec3 euler_ang;
    // *geodetic Euler angles
    // computed pitch angle: 'thtbdc'
    // note: when |tbd13| >= 1, thtbdc = +- pi/2, but cos(thtbdc) is
    // forced to be a small positive number to prevent division by zero
    if (fabs(tbd13) < 1) {
        thtbdc = asin(-tbd13);
        cthtbd = cos(thtbdc);
    } else {
        thtbdc = PI / 2 * sign(-tbd13);
        cthtbd = EPS;
    }
    // computed yaw angle: 'psibdc'
    double cpsi = tbd11 / cthtbd;
    if (fabs(cpsi) > 1)
        cpsi = 1 * sign(cpsi);
    psibdc = acos(cpsi) * sign(tbd12);

    // computed roll angle: 'phibdc'
    double cphi = tbd33 / cthtbd;
    if (fabs(cphi) > 1)
        cphi = 1 * sign(cphi);

    // selecting the Euler roll angle of flight mechanics (not for thtbdc=90 or
    // =-90deg)
    if (mroll == 0 || mroll == 1)
        // roll feedback for right side up
        phibdc = acos(cphi) * sign(tbd23);
    else if (mroll == 2)
        // roll feedback for inverted flight
        phibdc = acos(-cphi) * sign(-tbd23);

    euler_ang(0) = phibdc;
    euler_ang(1) = thtbdc;
    euler_ang(2) = psibdc;

    return euler_ang;
}

int sign(const double &variable)
{
    int sign = 0;
    if (variable < 0.)
        sign = -1;
    if (variable >= 0.)
        sign = 1;

    return sign;
}