#include "Dynamics_System.hpp"

Dynamics_Sys::Dynamics_Sys(double dt_In) {
    nbody = 0;
    njoint = 0;
    dt = dt_In;
}

void Dynamics_Sys::Add(Body *bodyPtr_In) {
    Body_ptr_array.push_back(bodyPtr_In);
    nbody++;
}

void Dynamics_Sys::Add(Joint *jointPtr_In) {
    Joint_ptr_array.push_back(jointPtr_In);
    njoint++;
}

void Dynamics_Sys::Cal_Constraints() {
    
    SYS_Cq.eye(6, 6);
    SYS_GAMMA = arma::zeros<arma::vec>(6, 1);
    SYS_M.eye(6, 6);
    SYS_C.zeros(0, 0);
    SYS_C = arma::join_cols(Body_ptr_array[0]->get_POSITION(), Body_ptr_array[0]->get_ANGLE());

    for (unsigned int i = 0; i < njoint; i++) {
        SYS_Cq = arma::join_cols(arma::join_rows(SYS_Cq, arma::zeros<arma::mat>(6 + i * 3, 6)),
            arma::join_rows(arma::zeros<arma::mat>(3, 6 * i), Joint_ptr_array[i]->get_Cq()));

        SYS_GAMMA = arma::join_cols(SYS_GAMMA, Joint_ptr_array[i]->get_GAMMA());

        SYS_C = arma::join_cols(SYS_C, Joint_ptr_array[i]->get_CONSTRAINT());
    }

    for (unsigned int i = 1; i < nbody; i++) {
        SYS_M = arma::join_cols(arma::join_rows(SYS_M, arma::zeros<arma::mat>(6 * i, 6)), 
            arma::join_rows(arma::zeros<arma::mat>(6, 6 * i), Body_ptr_array[i]->get_M()));
    }

    arma::vec tmp_vec;

    for (auto it_body = Body_ptr_array.begin(); it_body != Body_ptr_array.end(); it_body++) {
        
        tmp_vec = arma::join_cols(tmp_vec, arma::join_cols((*it_body)->get_FORCE(), (*it_body)->get_TORQUE()));
    }

    arma::vec tmp_vec2;
    tmp_vec2.zeros(0, 0);
    for (auto it = Body_ptr_array.begin(); it != Body_ptr_array.end(); ++it) {
        tmp_vec2 = arma::join_cols(tmp_vec2, (*it)->get_VELOCITY());
        tmp_vec2 = arma::join_cols(tmp_vec2, (*it)->get_ANGLE_VEL());
    }

    SYS_GAMMA = SYS_GAMMA - 2.0 * SYS_Cq * tmp_vec2 - SYS_C;

    SYS_RHS = arma::join_cols(tmp_vec, SYS_GAMMA);

    SYS_MAT = arma::join_cols(arma::join_rows(SYS_M, trans(SYS_Cq)), 
        arma::join_rows(SYS_Cq, arma::zeros<arma::mat>(SYS_Cq.n_rows, SYS_Cq.n_rows))); 
}

void Dynamics_Sys::Assembly() { //Need to be done, Initialization sequence!!!
    Body *i_ptr, *j_ptr;
    for (unsigned int i = 0; i < njoint; ++i) {
        i_ptr = Joint_ptr_array[i]->get_body_i_ptr();
        j_ptr = Joint_ptr_array[i]->get_body_j_ptr();

        j_ptr->set_POSITION(i_ptr->get_POSITION() + Joint_ptr_array[i]->get_Pi()
            - Joint_ptr_array[i]->get_Pj());
    }
}

void Dynamics_Sys::init() {
    for (auto it = Body_ptr_array.begin(); it != Body_ptr_array.end(); it++) {
        q.push_back((*it)->get_POSITION());
        q.push_back((*it)->get_VELOCITY());
        q.push_back((*it)->get_TBI_Q());
        q.push_back((*it)->get_ANGLE_VEL());

        q_d.push_back((*it)->get_VELOCITY());
        q_d.push_back((*it)->get_ACCELERATION());
        q_d.push_back((*it)->get_TBID_Q());
        q_d.push_back((*it)->get_ANGLE_ACC());
    }
    Cal_Constraints();
}

void Dynamics_Sys::solve() {
    std::vector<arma::vec> q_temp;
    std::vector<arma::vec> k1(q.size());
    std::vector<arma::vec> k2(q.size());
    std::vector<arma::vec> k3(q.size());
    std::vector<arma::vec> k4(q.size());
    
    q_temp = q;

    dynamic_function(q_temp, k1);
    for (unsigned int i = 0; i < q_temp.size(); i++) {
        q_temp[i] = q[i] + 0.5 * dt * k1[i];        
    }

    dynamic_function(q_temp, k2);
    for (unsigned int i = 0; i < q_temp.size(); i++) {
        q_temp[i] = q[i] + 0.5 * dt * k2[i];        
    }    

    dynamic_function(q_temp, k3);
    for (unsigned int i = 0; i < q_temp.size(); i++) {
        q_temp[i] = q[i] + dt * k3[i];        
    } 

    dynamic_function(q_temp, k4);
    for (unsigned int i = 0; i < q_temp.size(); i++) {
        q_d[i] = (1.0 / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
        q[i] = q[i] + q_d[i] * dt;        
    } 
    // std::cout << "POS:" << '\t' << q[4](0) << '\t' << q[4](1) << '\t' << q[4](2) << '\t';
    // std::cout << "ANG:" << '\t' << q[7](0) * 180.0 / 3.1415926 << '\t'
    //      << q[7](1) * 180.0 / 3.1415926 << '\t' << q[7](2) * 180.0 / 3.1415926 << std::endl;

}

void Dynamics_Sys::dynamic_function(std::vector<arma::vec> qIn, std::vector<arma::vec> &qdOut) {

    for (unsigned int i = 0; i < nbody; i++) {
        Body_ptr_array[i]->update(qIn[i * 4], qIn[(i * 4) + 1], qIn[(i * 4) + 2], qIn[(i * 4) + 3]);
    }

    for (auto it = Joint_ptr_array.begin(); it != Joint_ptr_array.end(); it++) {
        (*it)->update();
    }

    Cal_Constraints();

    arma::vec ANS;

    ANS = arma::solve(SYS_MAT, SYS_RHS);

    // for (auto it = ANS.begin(); it != ANS.end(); it++) std::cout << *it << std::endl;
    // for (unsigned int i = 0; i < SYS_MAT.n_rows; i++) {
    //    for (unsigned int j = 0; j < SYS_MAT.n_cols; j++) {
    //        std::cout << SYS_MAT(i, j) << '\t';
    //     }
    //     std::cout << std::endl;
    // }

    for (unsigned int i = 0; i < nbody; i++) {
        qdOut[(i * 4)] = qIn[(i * 4) + 1];
        qdOut[(i * 4) + 1] = ANS.subvec((i * 6), (i * 6)+2);
        qdOut[(i * 4) + 2] = Body_ptr_array[i]->get_TBID_Q();
        qdOut[(i * 4) + 3] = ANS.subvec((i * 6) + 3, (i * 6) + 5);
    }

    // for (auto it = qdOut.begin(); it != qdOut.end(); it++) {
    //     for (unsigned int i = 0; i < 3; i++) {
    //         std::cout << (*it)(i) << '\t';
    //     }
    //     std::cout << std::endl;
    // }
}
void Dynamics_Sys::output_data(std::ofstream &fout_In) {
    // arma::vec3 v_temp;
    for (unsigned int i = 1; i < nbody; i++) {
        // v_temp = Joint_ptr_array[i - 1]->get_Pj();
        fout_In << q[i * 4](0) << '\t' << q[i * 4](1) << '\t' << q[i * 4](2) << '\t';
    }
    fout_In << std::endl;
}

unsigned int Dynamics_Sys::get_nbody() { return nbody; }
unsigned int Dynamics_Sys::get_njoint() { return njoint; }
