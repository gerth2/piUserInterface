#include "lookup2d.h"


// Closed-loop Controller Tuning Constants
#define GAIN_kP  0.0
#define GAIN_kI  0.0
#define GAIN_kD  0.0

// Controller Output Limits
#define MAX_CTRL_EFFORT 100.0
#define MIN_CTRL_EFFORT -100.0

//Misc controller constants
#define Ts 0.1 //Sample Time (in seconds)

// Mechanical Constants
#define ENC_TO_MOTOR_GEAR_RATIO 1.0    // Ratio to convert encoder rotations to motor rotations
#define ARM_TO_MOTOR_GEAR_RATIO 1.0    // Ratio to convert arm rotations to motor rotations
#define PADDLE_LEVER_ARM_LENGTH_M = 0.45 // Lever-arm Distance from paddle contact point to arm rotational axis

//Electrical Constants
#define MOTOR_SUPPLY_VOLTAGE_V 12.0 // supply voltage to motor
#define MOTOR_kV 0.1  // Ratio of motor velocity (in radians/sec) to back EMF (in volts)
#define MOTOR_kT 0.1  // Ratio of motor current (in A) to motor shaft torque (in N/M)
#define MOTOR_R  0.01 // Motor winding resistance (in ohms)

//Pressure/Force Transforms for AmbiBag
const double pressureToArmForceMap[4][2] = 
// +-- Bag Output Pressure (in kPa)
// |     +-- Bag applied Force (N)
// |     |
{ {0,    0},
  {500,  0.25},
  {1000, 0.5 },
  {2000, 1.0 }, 
};

class CorePressureCtrl {

    //IO
    double desPress;
    double actPress;
    double encPos;
    double ctrlEffort;

    //State
    double encPosPrev = 0;
    double encVel_radPerSec = 0;
    double motorVel_radPerSec = 0;
    double motorBackEMF_V = 0;
    double desArmForce_Nm = 0;
    double desMotorTorque_Nm = 0;
    double desMotorCurrent_A = 0;
    double armForceError_Nm = 0;
    double motorCmdOL_V = 0;
    double motorCmdCL_V = 0;


    Lookup2D * pressureToArmForceLUT;

  public:

    CorePressureCtrl();

    // Input for the desired (command) pressure in kPa. Should come from some pre-defined inspiration profile, advanced by pressure events
    void setDesiredPressure(double des_kPa);

    // Input for the actual (measured) pressure in kPa. 
    void setActualPressure(double act_kPa);

    // Input for the present position of the encoder (in degrees)
    void setEncoderPosition(double encPos_deg);

    // Main controller step function. Call at rate TS.
    int update (void);

    // Output for the motor command
    double getCtrlEffortCmd(void);
};