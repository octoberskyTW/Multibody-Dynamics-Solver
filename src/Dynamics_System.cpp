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
    SYS_M.eye(6, 6);

    for (unsigned int i = 0; i < njoint; i++) {
        SYS_Cq = arma::join_cols(arma::join_rows(SYS_Cq, arma::zeros<arma::mat>(6 + i * 3, 6)),
            arma::join_rows(arma::zeros<arma::mat>(3, 6 * i), Joint_ptr_array[i]->get_Cq()));

        SYS_GAMMA = arma::join_cols(SYS_GAMMA, Joint_ptr_array[i]->get_GAMMA());
    }

    for (unsigned int i = 1; i < nbody; i++) {
        SYS_M = arma::join_cols(arma::join_rows(SYS_M, arma::zeros<arma::mat>(6 * i, 6)), 
            arma::join_rows(arma::zeros<arma::mat>(6, 6 * i), Body_ptr_array[i]->get_M()));
    }

    for (auto it_body = Body_ptr_array.begin(); it_body != Body_ptr_array.end(); it_body++) {
        
        SYS_Q = arma::join_cols(SYS_Q, arma::join_cols((*it_body)->get_FORCE(), (*it_body)->get_TORQUE()));
    }

    SYS_Q = arma::join_cols(SYS_Q, SYS_GAMMA);

    SYS_MAT = arma::join_cols(arma::join_rows(SYS_M, trans(SYS_Cq)), 
        arma::join_rows(SYS_Cq, arma::zeros<arma::mat>(SYS_Cq.n_rows, SYS_Cq.n_rows))); 
}

unsigned int Dynamics_Sys::get_nbody() { return nbody; }
unsigned int Dynamics_Sys::get_njoint() { return njoint; }