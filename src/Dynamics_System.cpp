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

    arma::vec tmp_vec;

    for (auto it_body = Body_ptr_array.begin(); it_body != Body_ptr_array.end(); it_body++) {
        
        tmp_vec = arma::join_cols(tmp_vec, arma::join_cols((*it_body)->get_FORCE(), (*it_body)->get_TORQUE()));
    }

    SYS_RHS = arma::join_cols(tmp_vec, SYS_GAMMA);

    SYS_MAT = arma::join_cols(arma::join_rows(SYS_M, trans(SYS_Cq)), 
        arma::join_rows(SYS_Cq, arma::zeros<arma::mat>(SYS_Cq.n_rows, SYS_Cq.n_rows))); 
}

void Dynamics_Sys::init() {
    Assembly();

    for (auto it = Body_ptr_array.begin(); it != Body_ptr_array.end(); it++) {
        q.push_back((*it)->get_POSITION());
        q.push_back((*it)->get_VELOCITY());
        q.push_back((*it)->get_ANGLE());
        q.push_back((*it)->get_ANGLE_VEL());

        q_d.push_back((*it)->get_VELOCITY());
        q_d.push_back((*it)->get_ACCELERATION());
        q_d.push_back((*it)->get_ANGLE_VEL());
        q_d.push_back((*it)->get_ANGLE_ACC());
    }
}

void Dynamics_Sys::solve() {
    dynamic_function(q, q_d);
}

void Dynamics_Sys::dynamic_function(std::vector<arma::vec> qIn, std::vector<arma::vec> &qdOut) {

    for (unsigned int i = 0; i < nbody; i++) {
        Body_ptr_array[i]->update(qIn[i * 4], qIn[(i * 4) + 1], qIn[(i * 4) + 2], qIn[(i * 4) + 3]);
    }

    for (auto it = Joint_ptr_array.begin(); it != Joint_ptr_array.end(); it++) {
        (*it)->update();
    }

    Assembly();

    arma::vec ANS;

    ANS = arma::solve(SYS_MAT, SYS_RHS);

    // for (auto it = ANS.begin(); it != ANS.end(); it++) std::cout << *it << std::endl;
    // for (unsigned int i = 0; i < SYS_MAT.n_rows; i++) {
    //     for (unsigned int j = 0; j < SYS_MAT.n_cols; j++) {
    //         std::cout << SYS_MAT(i, j) << '\t';
    //     }
    //     std::cout << std::endl;
    // }

    for (unsigned int i = 0; i < nbody; i++) {
        qdOut[(i * 4)] = qIn[(i * 4) + 1];
        qdOut[(i * 4) + 1] = qIn[(i * 4) + 3];
        qdOut[(i * 4) + 2] = ANS.subvec((i * 4), (i * 4)+2);
        qdOut[(i * 4) + 3] = ANS.subvec((i * 4) + 3, (i * 4) + 5);
    }

    // for (auto it = qdOut.begin(); it != qdOut.end(); it++) {
    //     for (unsigned int i = 0; i < 3; i++) {
    //         std::cout << (*it)(i) << '\t';
    //     }
    //     std::cout << std::endl;
    // }
}

unsigned int Dynamics_Sys::get_nbody() { return nbody; }
unsigned int Dynamics_Sys::get_njoint() { return njoint; }