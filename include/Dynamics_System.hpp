#ifndef DYNAMICS_HPP
#define DYNAMICS_HPP

#include "Body.hpp"
#include "Joint.hpp"
#include <vector>

class Dynamics_Sys
{
public:
    Dynamics_Sys();
    ~Dynamics_Sys() {};

    void Add(Body *bodyPtr_In);
    void Add(Joint *jointPtr_In);
    void Assembly();

    unsigned int get_nbody();
    unsigned int get_njoint();    
    
private:
    unsigned int nbody;
    unsigned int njoint;
    arma::mat SYS_Cq;
    arma::mat SYS_MAT;
    arma::vec SYS_C;
    arma::vec SYS_Q;
    arma::vec SYS_GAMMA;
    arma::vec q_dd;
    arma::vec q_d;
    arma::vec q;
    arma::mat SYS_M;

    std::vector<Body *> Body_ptr_array;
    std::vector<Joint *> Joint_ptr_array; 
};

#endif  //DYNAMICS_HPP