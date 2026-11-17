#include "vex.h"

using namespace vex;

float maxDec = 0.1; // maximum deceleration (percentage per ms)
float oldTrainLInput = 0.0;
float oldTrainRInput = 0.0;
float adjustedTrainLInput = 0.0;
float adjustedTrainRInput = 0.0;

bool clawSpin = false;

void pre_auton() {
  Lift.setStopping(hold);
}

void autonomous() {
}

void drivercontrol() {
  while (true) {
    // Get current inputs, scale left/right input for lower sensitivity
    float trainLInput = (Controller1.Axis3.value() + (Controller1.Axis1.value() * 2.0 / 3.0)) / 100.0;
    float trainRInput = (Controller1.Axis3.value() - (Controller1.Axis1.value() * 2.0 / 3.0)) / 100.0;

    // Clamp inputs to [-1, 1]
    trainLInput = fmin(fmax(trainLInput, -1.0), 1.0);
    trainRInput = fmin(fmax(trainRInput, -1.0), 1.0);

    // Clamp deceleration to prevent tipping
    adjustedTrainLInput = fmax(trainLInput, oldTrainLInput - maxDec * 20);
    adjustedTrainRInput = fmax(trainRInput, oldTrainRInput - maxDec * 20);

    // Get current target velocity for next iteration
    oldTrainLInput = adjustedTrainLInput;
    oldTrainRInput = adjustedTrainRInput;

    // Input curved to reduce sensitivity at low speeds
    int trainLrpm = 0.8 *(0.5 * adjustedTrainLInput * fabs(adjustedTrainLInput) + 0.5 * adjustedTrainLInput) * 600;
    int trainRrpm = 0.8 * (0.5 * adjustedTrainRInput * fabs(adjustedTrainRInput) + 0.5 * adjustedTrainRInput) * 600;


    DrivetrainL.spin(fwd, trainLrpm, rpm);
    DrivetrainR.spin(fwd, trainRrpm, rpm);

    //Flipper.spin(fwd, (Controller1.ButtonL1.pressing() - Controller1.ButtonL2.pressing()) * 200, rpm);
    Claw.spin(fwd, (Controller1.ButtonR2.pressing() - Controller1.ButtonR1.pressing()) * 200, rpm);
    Lift.spin(fwd, (Controller1.ButtonL1.pressing() - Controller1.ButtonL2.pressing()) * 200, rpm);

    this_thread::sleep_for(20);
  }
}

int main() {
  // Set up callbacks 
  Competition.autonomous(autonomous);
  Competition.drivercontrol(drivercontrol);

  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    this_thread::sleep_for(100);
  }
}
