#include "corePressureCtrl.h"

CorePressureCtrl::CorePressureCtrl(void){
    forcePIDController = new PIDController(Ts);
    forcePIDController->setP(GAIN_kP);
    forcePIDController->setI(GAIN_kI);
    forcePIDController->setD(GAIN_kD);

    pressureToArmForceLUT = new Lookup2D(pressureToArmForceMap, (unsigned int)(sizeof(pressureToArmForceMap)/sizeof(pressureToArmForceMap[0])));
}

void CorePressureCtrl::setDesiredPressure(double des_kPa){
    desPress = des_kPa;
}

void CorePressureCtrl::setActualPressure(double act_kPa){
    actPress = act_kPa;
}

void CorePressureCtrl::setEncoderPosition(double encPos_deg){
    encPos = encPos_deg;
}

int CorePressureCtrl::update(){
    //calculate encoder velocity - Discrete time derivative + conversion to rad/sec
    encVel_radPerSec = (3.14159/180.0) * (encPos - encPosPrev) * (1/Ts);

    //Calculate the desired compression force on the bag
    desArmForce_N = pressureToArmForceLUT->interpolate(desPress);

    //Open-loop control, including motor & gear mechanical model
    motorVel_radPerSec = encVel_radPerSec * ENC_TO_MOTOR_GEAR_RATIO;
    motorBackEMF_V = motorVel_radPerSec * MOTOR_kV;
    desMotorTorque_Nm = desArmForce_N * PADDLE_LEVER_ARM_LENGTH_M/ARM_TO_MOTOR_GEAR_RATIO;
    desMotorCurrent_A = desMotorTorque_Nm / MOTOR_kT;
    motorCmdOL_V = (desMotorCurrent_A * MOTOR_R) + motorBackEMF_V;

    //Closed-loop control with PID
    armForceError_Nm = desArmForce_N - pressureToArmForceLUT->interpolate(actPress);
    motorCmdCL_V = forcePIDController->update(armForceError_Nm);
    
    ctrlEffort = (motorCmdCL_V + motorCmdOL_V)/MOTOR_SUPPLY_VOLTAGE_V;

    // no error (yet)
    return 0; 
}

double CorePressureCtrl::getCtrlEffortCmd(void){
    return ctrlEffort; 
}