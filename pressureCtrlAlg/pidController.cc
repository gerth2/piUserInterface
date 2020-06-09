#include "pidController.h"

PIDController::PIDController(double Ts_in){
    errPrev = 0;
    errAccum = 0;
    kP = 0;
    kI = 0;
    kD = 0;
    Ts = Ts_in;
}

void PIDController::setP(double kP_in){
    kP = kP_in;
}

void PIDController::setI(double kI_in){
    kI = kI_in;
}

void PIDController::setD(double kD_in){
    kD = kD_in;
}

void PIDController::resetIntegrator(){
    errAccum = 0;
}

double PIDController::update(double err_input){

    errAccum += err_input * Ts;

    double errDeriv = (err_input - errPrev)/Ts;

    return (kP * err_input) + (kI * errAccum) + (kD * errDeriv);

}