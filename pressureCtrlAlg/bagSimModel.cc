#include "bagSimModel.h"

#define Ts 0.01
#define PWM_TO_POS_SCALE 0.01

#define BAG_MAX_VOL 1.0
#define BAG_MIN_VOL 0.25

//Expected flow vs. Pressure
// PSI        2      5      10    15   20   25   30
// kPa        13.7   34.5   68.9  103  138  172  206
// Flow (CFM) 40.5   63.5   79.6  108  126  144  162


BagSimModel::BagSimModel(double Ts_in){
    armPctCompression = 0;
}


double BagSimModel::update(double motorPWM){

    armPctCompression += motorPWM * Ts * PWM_TO_POS_SCALE;
    double bagVol = (BAG_MAX_VOL - BAG_MIN_VOL) * armPctCompression + BAG_MIN_VOL;



}