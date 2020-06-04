#include "corePressureCtrl.h"

CorePressureCtrl::CorePressureCtrl(void){

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
    encVel_radPerSec = (3.14159/180.0/Ts) * (encPos - encPosPrev);
    
    
    //TODO keep going here

    return 0; 
}

double CorePressureCtrl::getCtrlEffortCmd(void){
    return ctrlEffort; 
}