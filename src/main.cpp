#include "Dynamics_System.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
    Dynamics_Sys *sys;
    Ground *Ground_Body;
    Joint *Rev_joint_1;
    // Joint *Rev_joint_2;
    Body *Body_1;
    // Body *Body_2;

    sys = new Dynamics_Sys();

    Ground_Body = new Ground();

    arma::vec pi = {0., 0., 0.};
    arma::vec pj = {-1., 0., 0.};
    arma::vec qi = {0., 0., 0.};
    arma::vec qj = {0., 0., 0.};
    arma::vec POS = {1., 0., 0.};
    arma::vec VEL = {0., 0., 0.};
    arma::vec ACC = {0., 0., 0.};
    arma::vec ANG = {0., 0., 0.};
    arma::vec ANG_VEL = {0., 0., 0.};
    arma::vec ANG_ACC = {0., 0., 0.};
    arma::vec I = {1., 1., 1.};
    arma::vec F = {9.81, 0., 0.};
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
    arma::vec F_1 = {0., -9.81, 0.};
    arma::vec T_1 = {0., 0., 0.};

    Body_1 = new Mobilized_body(POS, VEL, ACC, ANG, ANG_VEL, ANG_ACC, 1.0, I, F, T);
    // Body_2 = new Mobilized_body(POS_1, VEL_1, ACC_1, ANG_1, ANG_VEL_1, ANG_ACC_1, 1.0, I_1, F_1, T_1);

    sys->Add(Ground_Body);
    sys->Add(Body_1);
    // sys->Add(Body_2);

    Rev_joint_1 = new Joint(0, pi, pj, qi, qj, Ground_Body, Body_1);
    // Rev_joint_2 = new Joint(0, pi_1, pj_1, qi_1, qj_1, Body_1, Body_2);

    sys->Add(Rev_joint_1);
    // sys->Add(Rev_joint_2);

    std::cout << sys->get_nbody() << '\t' << sys->get_njoint() << std::endl;

    sys->init();
    sys->solve();

    return 0;
}