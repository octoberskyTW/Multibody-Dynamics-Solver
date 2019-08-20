#ifndef BODY_HPP
#define BODY_HPP

#include <armadillo>
#include "Math.hpp"

class Body
{
public:
    Body();
    ~Body() {};

    arma::mat get_TBI();
    arma::vec get_POSITION();
    arma::vec get_VELOCITY();
    arma::vec get_ACCELERATION();
    arma::vec get_ANGLE_VEL();
    arma::vec get_ANGLE();
    arma::vec get_ANGLE_ACC();
    arma::vec get_FORCE();
    arma::vec get_TORQUE();
    arma::mat get_M();
    virtual void update(arma::vec PosIn, arma::vec VelIn, arma::vec AttIn
        , arma::vec ANG_VEL_In) = 0;

protected:
    arma::vec POSITION;
    arma::vec VELOCITY;
    arma::vec ACCELERATION;
    arma::vec ANGLE;
    arma::vec ANGLE_VEL;
    arma::vec ANGLE_ACC;
    arma::mat M;
    arma::vec FORCE;
    arma::vec TORQUE;
    arma::vec APPILED_TORQUE;
    arma::mat TBI;
};

class Ground : public Body
{
public:
    Ground();
    ~Ground() {};
    virtual void update(arma::vec PosIn, arma::vec VelIn, arma::vec AttIn
        , arma::vec ANG_VEL_In) {};
};

class Mobilized_body : public Body
{
public:
    Mobilized_body(arma::vec PosIn, arma::vec VelIn, arma::vec AccIn, arma::vec AttIn
        , arma::vec ANG_VEL_In, arma::vec ANG_ACC_In, double MIn, arma::vec IIn
        , arma::vec F_In, arma::vec T_In);
    ~Mobilized_body();

    virtual void update(arma::vec PosIn, arma::vec VelIn, arma::vec AttIn
        , arma::vec ANG_VEL_In) override;
};

#endif  //BODY_HPP