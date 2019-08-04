#ifndef PENDULUM_HPP
#define PENDULUM_HPP

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <armadillo>
#include <boost/bind.hpp>

using namespace boost::numeric::ublas;
using namespace boost::numeric::odeint;

class pendulum {
public:
    pendulum(double L, double m, double I, double ang);

    void sys(const arma::vec &x, arma::vec &dx);
    void build_TM(const arma::vec &angle, arma::mat &TM);
    arma::mat build_cross(const arma::vec &vec);

    void integrator();

    double dt;

    arma::mat M;
    arma::mat J;
    arma::mat ROT;
    arma::vec POS;
    arma::vec VEL;
    arma::vec ACC;
    arma::vec ANG;
    arma::vec ANG_VEL;
    arma::vec ANG_ACC;
    arma::vec F;
    arma::vec T;
    arma::mat PHI_Q;
    arma::vec Gamma;
    arma::vec Q;
    arma::mat MM;
    arma::vec state_var;
};
#endif
