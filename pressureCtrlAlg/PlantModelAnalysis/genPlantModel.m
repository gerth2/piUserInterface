%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Constants
MOTOR_SUPPLY_VOLTAGE_V = 12.0;
%ATMOSPHERIC_PRESSURE_MB = 1013.25;
ATMOSPHERIC_PRESSURE_MB = 0;

CAM_GEAR_RATIO = 30.0/48.0;
ENC_TICKS_PER_REV = 28;


GEARBOX_GEAR_RATIO = (192.0/1.0);
ENC_TO_MOTOR_GEAR_RATIO = 1.0;   % Ratio to convert encoder rotations to motor rotations
ARM_TO_MOTOR_GEAR_RATIO = GEARBOX_GEAR_RATIO * (48.0/30.0);  % Ratio to convert arm rotations to motor rotations
PADDLE_LEVER_ARM_LENGTH_M = 0.45; % Lever-arm Distance from paddle contact point to arm rotational axis

% See andymark.com/products/12-volt-dc-192-1-reduction-gearmotor-with-0-5-in-hex-output?sku=am-4252
MOTOR_STALL_TORQUE_NM = (40.67 / GEARBOX_GEAR_RATIO);
MOTOR_STALL_CURRENT_A = 11.0;
MOTOR_NOLOAD_SPEED_RADpS = 3.14159 * GEARBOX_GEAR_RATIO;
MOTOR_NOLOAD_CURRENT_A = 2.0;

MOTOR_R  = MOTOR_SUPPLY_VOLTAGE_V/MOTOR_STALL_CURRENT_A; % Motor winding resistance (in ohms). Stall Conditions: 
MOTOR_kV = (MOTOR_SUPPLY_VOLTAGE_V - MOTOR_NOLOAD_CURRENT_A*MOTOR_R)/MOTOR_NOLOAD_SPEED_RADpS; % Ratio to convert motor velocity into back-EMF
MOTOR_kT = MOTOR_STALL_TORQUE_NM/MOTOR_STALL_CURRENT_A; %Ratio to convert motor current into motor torque

% Account for sensor delay in data analysis
PRESSURE_OFFSET_SAMPLES = 7;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read in the data file
rawData = csvread('data_june30.csv');

% Separate raw CSV by the actual data components
% These were defined by the arduino code that generated the file
maxSample = 4500;
time_sec     = rawData(1:maxSample,1).*(1/1000);
flow_smLpM   = rawData(1:maxSample,2);
press_mb     = rawData(1:maxSample,3).*(0.98/1000);
cpuLoad      = rawData(1:maxSample,4);
encPos_raw   = rawData(1:maxSample,5);
temp_degC    = rawData(1:maxSample,6);
motorCmd_dir = rawData(1:maxSample,7); % 0 for fwd, 1 for rev
motorCmd_pwm = rawData(1:maxSample,8);
measCurrent_raw = rawData(1:maxSample,9);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Derived data
dt = diff(time_sec);
dEncPos_raw = diff(encPos_raw);
encVel_rawPSec = cat(1, dEncPos_raw./dt, [0]);
encVel_filt_rawPSec = filter([1,1,1,1,1], [5 0 0], encVel_rawPSec);

measCurrent_A = filter([1,1,1,1,1], [5 0 0], measCurrent_raw*10.0/1024);

press_filt_mb       = filter([1,1,1,1,1], [5 0 0], press_mb .- ATMOSPHERIC_PRESSURE_MB);

flow_filt_smLpM     = filter([1,1,1,1,1], [5 0 0], flow_smLpM);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calcualte arm force from measured parameters
motorIn_V = MOTOR_SUPPLY_VOLTAGE_V .* (1/255) .* motorCmd_pwm .* ( 2 .* (motorCmd_dir .+ -0.5));
motorVel_radPerSec = encVel_filt_rawPSec .* (CAM_GEAR_RATIO / ENC_TICKS_PER_REV * 2 * pi);
motorCurrent_A = (1.0/MOTOR_R) .* (motorIn_V - MOTOR_kV.*motorVel_radPerSec);
motorTorque_Nm = motorCurrent_A * MOTOR_kT;
armForce_N = motorTorque_Nm .* (GEARBOX_GEAR_RATIO / CAM_GEAR_RATIO * PADDLE_LEVER_ARM_LENGTH_M);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Plot Results
figure;
%plotyy(time_sec, armForce_N, time_sec, flow_filt_smLpM);
%legend('Arm Force (N)', 'Flow (smLpM)');
plotyy(time_sec, armForce_N, time_sec, press_filt_mb);
legend('Arm Force (N)', 'Pressure (mb)');

%subplot(2,1,1);
%plot(time_sec, armForce_N);
%legend();
%subplot(2,1,2);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calcualte averaged arm-force-to-pressure transform on compression stroke

% Down select only to samples where motor command is positive (compression)
maskVector = max(0, motorIn_V);
maskVector = min(0.01, maskVector).*100;
press_filt_masked_mb = maskVector .* press_filt_mb;
arm_force_masked_N   = maskVector .* armForce_N;

% Designed pressure-force lookup table
pressure_mb_breakpoints = [0.0, 10.0, 20.0, 30.0, 40.0, 50.0];
arm_force_N_breakpoints = [7.5,  9.3, 11.0, 13.5, 16.0, 18.0];

% Plot Results
figure;
hold on;
scatter(press_filt_masked_mb, arm_force_masked_N);
line(pressure_mb_breakpoints, arm_force_N_breakpoints, 'LineWidth',5);
hold off;

figure;
plot(time_sec, measCurrent_A, time_sec, motorCurrent_A);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Basic IO Boundary:
% Motor Voltage in, Pressure and Flow and Arm Position out
% PV = NrT
% r & T are constants
% V is proportional to arm position
% P is measured by sensors
% dN/dT
