#ifndef PENDULUM_HPP
#define PENDULUM_HPP

#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric::ublas;

class pendulum {
public:
    pendulum(double L, double m, double I, double ang);

    void pendulum_sys(vector<double> &x, vector<double> &dx, const double );
    void build_TM(const vector<double> &angle, matrix<double> &TM);
    matrix<double> build_cross(const vector<double> &vec);

    matrix<double> M;
    matrix<double> J;
    matrix<double> ROT;
    vector<double> POS;
    vector<double> VEL;
    vector<double> ACC;
    vector<double> ANG;
    vector<double> ANG_VEL;
    vector<double> ANG_ACC;
    vector<double> F;
    vector<double> T;
    matrix<double> PHI_Q;
    vector<double> Gamma;
};

template<class T>
bool InvertMatrix (const matrix<T>& input, matrix<T>& inverse) {

    typedef permutation_matrix<std::size_t> pmatrix;
    // create a working copy of the input
    matrix<T> A(input);
    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());

    // perform LU-factorization
    int res = lu_factorize(A,pm);
        if( res != 0 ) return false;

    // create identity matrix of "inverse"
    inverse.assign(identity_matrix<T>(A.size1()));

    // backsubstitute to get the inverse
    lu_substitute(A, pm, inverse);

    return true;
 }

#endif