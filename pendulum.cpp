#include "pendulum.hpp"

pendulum::pendulum(double L, double m, double I, double ang) :
    M(3, 3, arma::fill::zeros), ROT(2, 2, arma::fill::zeros), POS(2, arma::fill::zeros), VEL(2, arma::fill::zeros), ACC(2, arma::fill::zeros), F(2, arma::fill::zeros),
    T(1, arma::fill::zeros), ANG(1, arma::fill::zeros), ANG_VEL(1, arma::fill::zeros), ANG_ACC(1, arma::fill::zeros), PHI_Q(2, 3, arma::fill::zeros), Gamma(2, arma::fill::zeros), Q(5, arma::fill::zeros),
    state_var(6, arma::fill::zeros) {

        POS(1) = L;

        ANG(0) = ang;

        F(1) = m * 9.81;

        ANG_VEL(0) = 0.0;

        for (unsigned int i = 0; i < POS.size(); ++i) {
            M(i, i) = m;
        }

        M(2, 2) = I;

        build_TM(ANG, ROT);
        POS = trans(ROT) * POS;

        T = 0.0;
        Q = join_cols(F, T);
        dt = 0.001;

        state_var = join_cols(join_cols(POS, ANG), join_cols(VEL, ANG_VEL));
}

void pendulum::build_TM(const arma::vec &angle, arma::mat &TM) {
    double sang = sin(angle(0));
    double cang = cos(angle(0));

    TM(0, 0) = cang;
    TM(0, 1) = -sang;
    TM(1, 0) = sang;
    TM(1, 1) = cang;

    return;
}

arma::mat pendulum::build_cross(const arma::vec &vec) {
    arma::mat tmp(3, 3, arma::fill::zeros);

    tmp(0, 1) = -vec(2);
    tmp(0, 2) = vec(1);
    tmp(1, 0) = vec(2);
    tmp(1, 2) = -vec(0);
    tmp(2, 0) = -vec(1);
    tmp(2, 1) = vec(0);

    return tmp;
}

void pendulum::sys (const arma::vec &x, arma::vec &dx) {

    PHI_Q(0, 0) = 1.0;
    PHI_Q(0, 1) = 0.0;
    PHI_Q(0, 2) = sqrt(x(0) * x(0) + x(1) * x(1)) * sin(x(2));
    PHI_Q(1, 0) = 0.0;
    PHI_Q(1, 1) = 1.0;
    PHI_Q(1, 2) = -sqrt(x(0) * x(0) + x(1) * x(1)) * cos(x(2));

    Gamma(0) = -sqrt(x(0) * x(0) + x(1) * x(1)) * cos(x(2)) * x(5) * x(5);
    Gamma(1) = -sqrt(x(0) * x(0) + x(1) * x(1)) * sin(x(2)) * x(5) * x(5);

    arma::mat A;
    arma::mat tmp(2, 2, arma::fill::zeros);

    A = join_cols(join_rows(M, trans(PHI_Q)), join_rows(PHI_Q, tmp));

    Q = join_cols(join_cols(F, T), Gamma);

    arma::vec tmp_v(5, arma::fill::zeros);
    tmp_v = solve(A, Q);

    dx(0) = x(3);
    dx(1) = x(4);
    dx(2) = x(5);
    dx(3) = tmp_v(0);
    dx(4) = tmp_v(1);
    dx(5) = tmp_v(2);
}

void pendulum::integrator() {
    unsigned int n_elements = state_var.size();
    arma::vec xx(n_elements, arma::fill::zeros);
    arma::vec dx1(n_elements, arma::fill::zeros);
    arma::vec dx2(n_elements, arma::fill::zeros);
    arma::vec dx3(n_elements, arma::fill::zeros);
    arma::vec dx4(n_elements, arma::fill::zeros);

    xx = state_var;

    sys(xx, dx1);

    xx = state_var + dx1 * 0.5 * dt;

    sys(xx, dx2);

    xx = state_var + dx2 * 0.5 * dt;

    sys(xx, dx3);

    xx = state_var + dx3 * dt;

    sys(xx, dx4);

    state_var = state_var + (dt / 6.0) * (dx1 + 2.0 * dx2+ 2.0 * dx3 + dx4);
}
