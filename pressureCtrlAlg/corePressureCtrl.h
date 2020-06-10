#include "lookup2d.h"
#include "pidController.h"


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
#define GEARBOX_GEAR_RATIO (192.0/1.0)
#define ENC_TO_MOTOR_GEAR_RATIO 1.0    // Ratio to convert encoder rotations to motor rotations
#define ARM_TO_MOTOR_GEAR_RATIO GEARBOX_GEAR_RATIO * (48.0/30.0)  // Ratio to convert arm rotations to motor rotations
#define PADDLE_LEVER_ARM_LENGTH_M 0.45 // Lever-arm Distance from paddle contact point to arm rotational axis

//Electrical Constants
#define MOTOR_SUPPLY_VOLTAGE_V 12.0 // supply voltage to motor
// See andymark.com/products/12-volt-dc-192-1-reduction-gearmotor-with-0-5-in-hex-output?sku=am-4252
#define MOTOR_STALL_TORQUE_NM (40.67 / GEARBOX_GEAR_RATIO)
#define MOTOR_STALL_CURRENT_A 11.0
#define MOTOR_NOLOAD_SPEED_RADpS 3.14159 * GEARBOX_GEAR_RATIO
#define MOTOR_NOLOAD_CURRENT_A 2.0
// Derived electrical constants
#define MOTOR_R  MOTOR_SUPPLY_VOLTAGE_V/MOTOR_STALL_CURRENT_A // Motor winding resistance (in ohms). Stall Conditions: 
#define MOTOR_kV (MOTOR_SUPPLY_VOLTAGE_V - MOTOR_NOLOAD_CURRENT_A*MOTOR_R)/MOTOR_NOLOAD_SPEED_RADpS  // Ratio to convert motor velocity into back-EMF
#define MOTOR_kT MOTOR_STALL_TORQUE_NM/MOTOR_STALL_CURRENT_A // Ratio to convert motor current into motor torque

//Pressure/Force Transforms for AmbiBag
const double pressureToArmForceMap[4][2] = 
// +-- Bag Output Pressure (in kPa)
// |     +-- Bag applied Force (N)
// |     |
{ {0,    0},
  {500,  50},
  {1000, 100 },
  {2000, 150 }, 
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
    double desArmForce_N = 0;
    double desMotorTorque_Nm = 0;
    double desMotorCurrent_A = 0;
    double armForceError_Nm = 0;
    double motorCmdOL_V = 0;
    double motorCmdCL_V = 0;


    Lookup2D * pressureToArmForceLUT;
    PIDController * forcePIDController;

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