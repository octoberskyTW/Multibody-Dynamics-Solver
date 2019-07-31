#include "pendulum.hpp"

pendulum::pendulum(double L, double m, double I, double ang) :
    M(3, 3, 0.0), J(3, 3, 0.0), ROT(3, 3, 0.0), POS(3, 0.0), VEL(3, 0.0), ACC(3, 0.0), F(3, 0.0),
    T(3, 0.0), ANG(3, 0.0), ANG_VEL(3, 0.0), ANG_ACC(3, 0.0), PHI_Q(6, 6, 0.0), Gamma(6, 0.0) {

        POS(2) = L;

        ANG(1) = ang;

        F(2) = m * 9.81;

        ANG_VEL(1) = 10.0;

        for (unsigned int i = 0; i < POS.size(); ++i)
        {
            M(i, i) = m;
            J(i, i) = I;
        }

        build_TM(ANG, ROT);
        POS = prod(trans(ROT), POS);
        matrix<double> tmp(3, 3, 0.0);
        vector<double> tmp_v(3, 0.0);
        tmp_v = prod(J, ANG_VEL);
        tmp = build_cross(ANG_VEL);
        T = prod(build_cross(POS), F) - prod(J, ANG_ACC) - prod(tmp, tmp_v);
}

void pendulum::build_TM(const vector<double> &angle, matrix<double> &TM) {
    double spsi = sin(angle(2));
    double cpsi = cos(angle(2));
    double stht = sin(angle(1));
    double ctht = cos(angle(1));
    double sphi = sin(angle(0));
    double cphi = cos(angle(0));

    TM(0, 0) = cpsi * ctht;
    TM(1, 0) = cpsi * stht * sphi - spsi * cphi;
    TM(2, 0) = cpsi * stht * cphi + spsi * sphi;
    TM(0, 1) = spsi * ctht;
    TM(1, 1) = spsi * stht * sphi + cpsi * cphi;
    TM(2, 1) = spsi * stht * cphi - cpsi * sphi;
    TM(0, 2) = -stht;
    TM(1, 2) = ctht * sphi;
    TM(2, 2) = ctht * cphi;

    return;
}

matrix<double> pendulum::build_cross(const vector<double> &vec) {
    matrix<double> tmp(3, 3, 0.0);

    tmp(0, 1) = -vec(2);
    tmp(0, 2) = vec(1);
    tmp(1, 0) = vec(2);
    tmp(1, 2) = -vec(0);
    tmp(2, 0) = -vec(1);
    tmp(2, 1) = vec(0);

    return tmp;
}

void pendulum::pendulum_sys(vector<double> &x, vector<double> &dx, const double ) {

    for (unsigned int i = 0; i < PHI_Q.size1(); ++i)
    {
        PHI_Q(i, i) = 1.0;
    }

    PHI_Q(0, 4) = -norm_2(POS) * cos(ANG(1));
    PHI_Q(1, 4) = norm_2(POS) * sin(ANG(1));

    Gamma(0) = norm_2(POS) * sin(ANG(1)) * ANG_VEL(1) * ANG_VEL(1);
    Gamma(1) = norm_2(POS) * cos(ANG(1)) * ANG_VEL(1) * ANG_VEL(1);

    matrix<double> tmp_m(6, 6, 0.0);

    InvertMatrix(PHI_Q, tmp_m);

    for (int i = 0; i < tmp_m.size1(); ++i)
    {
        std::cout << tmp_m(i, 0) << '\t' << tmp_m(i, 1) << '\t' << tmp_m(i, 2) << '\t' << tmp_m(i, 3) << '\t' << tmp_m(i, 4) << '\t' << tmp_m(i, 5) << "|" << Gamma(i) << std::endl;
    }

    vector<double> tmp_v(6, 0.0);

    tmp_v = prod(tmp_m, Gamma);

    std::cout << tmp_v(0) << '\t' << tmp_v(1) << '\t' << tmp_v(2) << '\t' << tmp_v(3) << '\t' << tmp_v(4) << '\t' << tmp_v(5) << std::endl;

    x = tmp_v;
}