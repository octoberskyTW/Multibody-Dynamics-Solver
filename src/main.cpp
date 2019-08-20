#include "Dynamics_System.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char const *argv[])
{
    Dynamics_Sys *sys;
    Ground *Ground_Body;
    Joint *Rev_joint_1;
    Joint *Rev_joint_2;
    Joint *Rev_joint_3;
    Joint *Rev_joint_4;

    Body *Body_1;
    Body *Body_2;
    Body *Body_3;
    Body *Body_4;
    double mass = 1.0;
    double time = 0.;

    std::ofstream fout;

    fout.open("data.txt");

    sys = new Dynamics_Sys(0.001);

    Ground_Body = new Ground();

    arma::vec pi = {0., 0., 0.};
    arma::vec pj = {-1., 0., 0.};
    arma::vec qi = {0., 0., 0.};
    arma::vec qj = {0., 0., 0.};
    arma::vec POS = {1., 0., 0.};
    arma::vec VEL = {0., 0., 0.};
    arma::vec ACC = {0., 0., 0.};
    arma::vec ANG = {0., -0. * 3.1415926 / 180.0, 0.};
    arma::vec ANG_VEL = {0., 0., 0.};
    arma::vec ANG_ACC = {0., 0., 0.};
    arma::vec I = {1., 1., 1.};
    arma::vec F = {0., 0., mass * 9.81};
    arma::vec T = {0., 0., 0.};
    arma::vec pi_1 = {1., 0., 0.};
    arma::vec pj_1 = {-1., 0., 0.};
    arma::vec qi_1 = {0., 0., 0.};
    arma::vec qj_1 = {0., 0., 0.};
    arma::vec POS_1 = {3., 0., 0.};
    arma::vec VEL_1 = {0., 0., 0.};
    arma::vec ACC_1 = {0., 0., 0.};
    arma::vec ANG_1 = {0., 0., 0.};
    arma::vec ANG_VEL_1 = {0., 0., 0.};
    arma::vec ANG_ACC_1 = {0., 0., 0.};
    arma::vec I_1 = {1., 1., 1.};
    arma::vec F_1 = {0., 0., mass * 9.81};
    arma::vec T_1 = {0., 0., 0.};

    arma::vec L = {2., 0., 0.};

    Body_1 = new Mobilized_body(POS, VEL, ACC, ANG, ANG_VEL, ANG_ACC, mass, I, F, T);
    Body_2 = new Mobilized_body(POS_1, VEL_1, ACC_1, ANG_1, ANG_VEL_1, ANG_ACC_1, mass, I_1, F_1, T_1);
    POS_1 = POS_1 + L;
    Body_3 = new Mobilized_body(POS_1, VEL_1, ACC_1, ANG_1, ANG_VEL_1, ANG_ACC_1, mass, I_1, F_1, T_1);
    POS_1 = POS_1 + L;
    Body_4 = new Mobilized_body(POS_1, VEL_1, ACC_1, ANG_1, ANG_VEL_1, ANG_ACC_1, mass, I_1, F_1, T_1);


    sys->Add(Ground_Body);
    sys->Add(Body_1);
    sys->Add(Body_2);
    sys->Add(Body_3);
    sys->Add(Body_4);

    Rev_joint_1 = new Joint(0, pi, pj, qi, qj, Ground_Body, Body_1);
    Rev_joint_2 = new Joint(0, pi_1, pj_1, qi_1, qj_1, Body_1, Body_2);
    Rev_joint_3 = new Joint(0, pi_1, pj_1, qi_1, qj_1, Body_2, Body_3);
    Rev_joint_4 = new Joint(0, pi_1, pj_1, qi_1, qj_1, Body_3, Body_4);

    sys->Add(Rev_joint_1);
    sys->Add(Rev_joint_2);
    sys->Add(Rev_joint_3);
    sys->Add(Rev_joint_4);

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
