#include "Dynamics_System.hpp"

Dynamics_Sys::Dynamics_Sys() {
    nbody = 0;
    njoint = 0;
}

void Dynamics_Sys::Add(Body *bodyPtr_In) {
    Body_ptr_array.push_back(bodyPtr_In);
    nbody++;
}

void Dynamics_Sys::Add(Joint *jointPtr_In) {
    Joint_ptr_array.push_back(jointPtr_In);
    njoint++;
}

void Dynamics_Sys::Assembly() {
    SYS_Cq.eye(6, 6);
    SYS_GAMMA = arma::zeros<arma::vec>(6, 1);

    for (unsigned int i = 0; i < njoint; i++) {
        SYS_Cq = arma::join_cols(arma::join_rows(SYS_Cq, arma::zeros<arma::mat>(6 + i * 3, 6)),
            arma::join_rows(arma::zeros<arma::mat>(3, 6 * i), Joint_ptr_array[i]->get_Cq()));

        SYS_GAMMA = arma::join_cols(SYS_GAMMA, Joint_ptr_array[i]->get_GAMMA());
    }

    for (unsigned int i = 0; i < SYS_Cq.n_rows; i++) {
        for (unsigned int j = 0; j < SYS_Cq.n_cols; j++) {
            std::cout << SYS_Cq(i, j) << '\t';
        }
        std::cout << std::endl;
    }
}

unsigned int Dynamics_Sys::get_nbody() { return nbody; }
unsigned int Dynamics_Sys::get_njoint() { return njoint; }