#include <fstream>
#include "pendulum.hpp"
#include <iostream>

using namespace boost::numeric::ublas;


int main(int argc, char const *argv[])
{
    double t(0.0);

    std::fstream fout;

    fout.open("test.txt", std::ios::out);

    fout << "Time" << '\t' << "X" << '\t' << "Y" << '\t' << "ANG" << std::endl;

    pendulum *A = new pendulum(1.0, 1.0, 1.0, 180.0 * 3.14 / 180.0);

    for (int i = 0; i < 100000; ++i)
    {
        A->integrator();
        t += A->dt;
        fout << t << '\t' << A->state_var(0) << '\t' << A->state_var(1) << '\t' << A->state_var(2) << std::endl;
    }

    fout.close();
    return 0;
}
