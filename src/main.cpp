#include "Dynamics_System.hpp"
#include <boost/make_shared.hpp>
#include <iostream>
#include <fstream>

int main(int argc, char const *argv[])
{
    Dynamics_Sys *sys;
    boost::shared_ptr<Ground> Ground_Body;
    JointPtr Rev_joint_1;
    JointPtr Rev_joint;

    BodyPtr Body_1;
    BodyPtr Body_prev;
    BodyPtr Body_now;
    double mass = 1.0;
    double time = 0.;

    std::ofstream fout;

    fout.open("data.csv");

    sys = new Dynamics_Sys(0.001);

    arma::vec pi = {0., 0., 0.};
    arma::vec pj = {-1., 0., 0.};
    arma::vec qi = {0., 0., 0.};
    arma::vec qj = {0., 0., 0.};
    arma::vec POS = {0., 0., 0.};
    arma::vec VEL = {0., 0., 0.};
    arma::vec VEL1 = {0., 0., 0.};
    arma::vec ACC = {0., 0., 0.};
    arma::vec ANG = {-0. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0};
    arma::vec ANG1 = {-0. * 3.1415926 / 180.0, -3. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0};
    arma::vec ANG_VEL = {0., 0., 0.};
    arma::vec ANG_VEL1 = {0., 0., 0.};
    arma::vec ANG_ACC = {0., 0., 0.};
    arma::vec I = {1., 1., 1.};
    arma::vec F = {0., 0., 9.8};
    arma::vec T = {0., 0., 0.};
    arma::vec T1 = {0., 0., 0.};

    Ground_Body = boost::make_shared<Ground>(0);
    Body_1 = boost::make_shared<Mobilized_body>(1, POS, VEL, ACC, ANG, ANG_VEL, ANG_ACC, mass, I, F, T);
    Rev_joint_1 = boost::make_shared<Joint>(0, pi, pj, qi, qj, Ground_Body, Body_1);
    sys->Add(Ground_Body);
    sys->Add(Body_1);
    sys->Add(Rev_joint_1);
    
    Body_prev = Body_1;
    for (unsigned int i = 0; i < 10; i++) {
        Body_now = boost::make_shared<Mobilized_body>(i + 2, POS, VEL1, ACC, ANG1, ANG_VEL1, ANG_ACC, mass, I, F, T);
        Rev_joint = boost::make_shared<Joint>(0, pi, pj, qi, qj, Body_prev, Body_now);

        sys->Add(Body_now);
        sys->Add(Rev_joint);
        Body_prev = Body_now;
    }
    sys->Assembly();
    sys->init();

    for (unsigned int i = 0; i < 50000; i++) {
        sys->solve();
        
        time += 0.001;

        if (i % 1 == 0) {
            fout << time << '\t';
            sys->output_data(fout);
        }
        
    } 


    fout.close();
    return 0;
}
