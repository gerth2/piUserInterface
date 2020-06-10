#include "corePressureCtrl.h"
#include <iostream>
#include <vector>

#define SIM_DUR_S 10.0
#define SIM_TS 0.01

int main()
{

  // Prepare data.
  int n = SIM_DUR_S / SIM_TS;
  std::vector<double> time(n), motorPWM(n);

  CorePressureCtrl ctrlUnderTest;

  // Execute simulation
  for (int i = 0; i < n; ++i)
  {
    time.at(i) = i * SIM_TS;

    //Calculate inputs
    ctrlUnderTest.setActualPressure(0);
    ctrlUnderTest.setDesiredPressure(0);
    ctrlUnderTest.setEncoderPosition(0);

    ctrlUnderTest.update();
    motorPWM.at(i) = ctrlUnderTest.getCtrlEffortCmd();

    std::cout << motorPWM.at(i);
    std::cout << "\n";
  }

  return 0;
}