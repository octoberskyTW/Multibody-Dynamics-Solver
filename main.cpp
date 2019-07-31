#include <fstream>
#include "pendulum.hpp"
#include <iostream>

using namespace boost::numeric::ublas;
using namespace boost::numeric::odeint;

int main(int argc, char const *argv[])
{
    double dt(0.001);

    double t(0.0);

    vector<double> X(6, 0.0);
    vector<double> DX(6, 0.0);

    pendulum A(10.0, 5.0, 1.0, 45.0 * 3.14 / 180.0);

    A.pendulum_sys(X, DX, t);
    // std::cout << X(3) << '\t' << X(4) << '\t' << X(5) << std::endl;
    return 0;
}