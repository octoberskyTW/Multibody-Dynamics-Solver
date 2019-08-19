#include "Body.hpp"

Body::Body() :
    POSITION(3, arma::fill::zeros),
    VELOCITY(3, arma::fill::zeros),
    ACCELERATION(3, arma::fill::zeros),
    ANGLE(3, arma::fill::zeros),
    ANGLE_VEL(3, arma::fill::zeros),
    ANGLE_ACC(3, arma::fill::zeros),
    M(6, 6, arma::fill::eye),
    FORCE(3, arma::fill::zeros),
    TORQUE(3, arma::fill::zeros),
    TBI(3, 3, arma::fill::zeros) {
}

arma::vec Body::get_POSITION() { return POSITION; }
arma::mat Body::get_TBI() { return TBI; }
arma::vec Body::get_ANGLE_VEL() { return ANGLE_VEL; }
arma::vec Body::get_ANGLE() { return ANGLE; }
arma::vec Body::get_VELOCITY() { return VELOCITY; }
arma::vec Body::get_ACCELERATION() { return ACCELERATION; }
arma::vec Body::get_ANGLE_ACC() { return ANGLE_ACC; }
arma::vec Body::get_FORCE() { return FORCE; }
arma::vec Body::get_TORQUE() { return TORQUE; }
arma::mat Body::get_M() { return M; }

Ground::Ground() {
    for (unsigned int i = 0; i < 3; i++) {
        M(i, i) = 1.0;
        M(i + 3, i + 3) = 1.0;
    }

    TBI = build_psi_tht_phi_TM(ANGLE(2), ANGLE(1), ANGLE(0));
}

Mobilized_body::Mobilized_body(arma::vec PosIn, arma::vec VelIn, arma::vec AccIn, arma::vec AttIn
        , arma::vec ANG_VEL_In, arma::vec ANG_ACC_In, double MIn, arma::vec IIn
        , arma::vec F_In, arma::vec T_In) {
    POSITION = PosIn;
    VELOCITY = VelIn;
    ACCELERATION = AccIn;
    ANGLE = AttIn;
    ANGLE_VEL = ANG_VEL_In;
    ANGLE_ACC = ANG_ACC_In;
    FORCE = F_In;
    TORQUE = T_In;
    for (unsigned int i = 0; i < 3; i++) {
        M(i, i) = MIn;
        M(i + 3, i + 3) = IIn(i);
    }

    TBI = build_psi_tht_phi_TM(ANGLE(2), ANGLE(1), ANGLE(0));
}

void Mobilized_body::update(arma::vec PosIn, arma::vec VelIn, arma::vec AttIn
        , arma::vec ANG_VEL_In) {

    POSITION = PosIn;
    VELOCITY = VelIn;
    ANGLE = AttIn;
    ANGLE_VEL = ANG_VEL_In;

    TBI = build_psi_tht_phi_TM(ANGLE(2), ANGLE(1), ANGLE(0));
}