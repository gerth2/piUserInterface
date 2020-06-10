#include "corePressureCtrl.h"
#include <iostream>
#include <iostream>
#include <fstream>
using namespace std;

#include <vector>

#define SIM_DUR_S 10.0
#define SIM_TS 0.01

int main()
{

  //Open log file with headers
  ofstream resultsFile;
  resultsFile.open ("results.csv");
  resultsFile << "TIME,actPressure,desPressure,encPos,motorPWM" << endl;
  resultsFile << "sec,kPa,kPa,deg,pct" << endl;
  
  // Prepare data.
  int n = SIM_DUR_S / SIM_TS;
  vector<double> time(n), motorPWM(n), actPress(n), desPress(n), encPos(n);

  CorePressureCtrl ctrlUnderTest;

  //Create input profile
  for (int i = 0; i < n; ++i)
  {
    time.at(i) = i * SIM_TS;

    actPress.at(i) = 0;
    encPos.at(i) = 0;

    //shifted basic triangle wave.... rough approx of breath cycle?
    if (time.at(i) >= 0.0 && time.at(i) < 2.5) {
      desPress.at(i) = (time.at(i)) * 1000;
    } else if (time.at(i) >= 2.5 && time.at(i) < 4.5){
      desPress.at(i) = 2000.0 - ((time.at(i)-2.5) * 1000);
    } else {
      desPress.at(i) = 0;
    }

  }

  // Execute simulation
  for (int i = 0; i < n; ++i)
  {
    //Calculate inputs
    ctrlUnderTest.setActualPressure(actPress.at(i));
    ctrlUnderTest.setDesiredPressure(desPress.at(i));
    ctrlUnderTest.setEncoderPosition(encPos.at(i));

    ctrlUnderTest.update();
    motorPWM.at(i) = ctrlUnderTest.getCtrlEffortCmd();


    //add line to log file
    resultsFile << time.at(i) << ",";
    resultsFile << actPress.at(i) << ",";
    resultsFile << desPress.at(i) << ",";
    resultsFile << encPos.at(i) << ",";
    resultsFile << motorPWM.at(i) << ",";
    resultsFile << endl;
  }

  //Close out results file
  resultsFile.close();
  return 0;
}