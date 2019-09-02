#include "Dynamics_System.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char const *argv[])
{
    Dynamics_Sys *sys;
    Ground *Ground_Body;
    Joint *Rev_joint_1;
    Joint *Rev_joint;

    Body *Body_1;
    Body *Body_prev;
    Body *Body_now;
    double mass = 1.0;
    double time = 0.;

    std::ofstream fout;

    fout.open("data.dat");

    sys = new Dynamics_Sys(0.001);

    arma::vec pi = {0., 0., 0.};
    arma::vec pj = {-1., 0., 0.};
    arma::vec qi = {0., 0., 0.};
    arma::vec qj = {0., 0., 0.};
    arma::vec POS = {0., 0., 0.};
    arma::vec VEL = {0., 0., 0.};
    arma::vec ACC = {0., 0., 0.};
    arma::vec ANG = {-0. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0};
    arma::vec ANG1 = {-0. * 3.1415926 / 180.0, -90. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0};
    arma::vec ANG_VEL = {0., 0., 0.};
    arma::vec ANG_ACC = {0., 0., 0.};
    arma::vec I = {1., 1., 1.};
    arma::vec F = {0., 0., mass * 9.81};
    arma::vec T = {0., 0., 0.};
    arma::vec pi_1 = {0., 0., 0.};
    arma::vec pj_1 = {-1., 0., 0.};
    arma::vec qi_1 = {0., 0., 0.};
    arma::vec qj_1 = {0., 0., 0.};

    Ground_Body = new Ground(POS);
    Body_1 = new Mobilized_body(POS, VEL, ACC, ANG, ANG_VEL, ANG_ACC, mass, I, F, T);
    Rev_joint_1 = new Joint(0, pi, pj, qi, qj, Ground_Body, Body_1);
    sys->Add(Ground_Body);
    sys->Add(Body_1);
    sys->Add(Rev_joint_1);
    
    Body_prev = Body_1;
    for (unsigned int i = 0; i < 30; i++) {
        Body_now = new Mobilized_body(POS, VEL, ACC, ANG1, ANG_VEL, ANG_ACC, mass, I, F, T);
        Rev_joint = new Joint(0, pi_1, pj_1, qi_1, qj_1, Body_prev, Body_now);

        sys->Add(Body_now);
        sys->Add(Rev_joint);
        Body_prev = Body_now;
    }
    sys->Assembly();
    sys->init();

    for (unsigned int i = 0; i < 50000; i++) {
        sys->solve();
        
        time += 0.001;

        fout << time << '\t';

        sys->output_data(fout);
    } 


    fout.close();
    return 0;
}
