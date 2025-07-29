#ifndef DYNAMICS_HPP
#define DYNAMICS_HPP

#include "Body.hpp"
#include "Joint.hpp"

#include <vector>

class Dynamics_Sys {
  public:
    Dynamics_Sys(double dt_In);
    ~Dynamics_Sys(){};

    void Add(BodyPtr bodyPtr_In);
    void Add(JointPtr jointPtr_In);
    void Cal_Constraints();
    void Assembly();
    void init();
    void solve();
    void output_data(std::ofstream& fout_In);

    unsigned int get_nbody();
    unsigned int get_njoint();

    std::vector<arma::vec> get_body_positions();
    std::vector<arma::vec> get_body_angles();

  private:
    void dynamic_function(std::vector<arma::vec> qIn, std::vector<arma::vec>& qdOut);

    double dt;
    unsigned int nbody;
    unsigned int njoint;
    arma::mat SYS_Cq;
    arma::mat SYS_MAT;
    arma::vec SYS_C;
    arma::vec SYS_RHS;
    arma::vec SYS_GAMMA;
    arma::vec q_dd;
    std::vector<arma::vec> q_d;
    std::vector<arma::vec> q;
    arma::mat SYS_M;

    std::vector<BodyPtr> Body_ptr_array;
    std::vector<JointPtr> Joint_ptr_array;
};

#endif // DYNAMICS_HPP