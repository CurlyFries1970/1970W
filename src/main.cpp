#include "vex.h"

using namespace vex;

float auton::functionalCircumference = (4.0 / 3.0) * 3.0 * 3.1415926;

float maxDec = 0.02; // maximum deceleration (percentage per ms)
float oldTrainLInput = 0.0;
float oldTrainRInput = 0.0;
float adjustedTrainLInput = 0.0;
float adjustedTrainRInput = 0.0;

int autonMode = 0;
void switchAuton() {
  autonMode = (autonMode + 1) % 3; 
}

void pre_auton() {
  Lift.setStopping(hold);
  ArmEncoder.setPosition(0, deg);

  Controller1.ButtonA.pressed(switchAuton);
}

//Autons
void near() {
  auton::straight(16.0, 1.00);
  auton::turn(-0.25);
  //Will require live tweaks on the day of competition
}

void far() {}

void skills() {}
/////

void autonomous() {
  if (autonMode == 0) {
    near();
  } else if (autonMode == 1) {
    far();
  } else {
    skills();
  }
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
    adjustedTrainLInput = fmax(trainLInput, oldTrainLInput - maxDec * 20.0);
    adjustedTrainRInput = fmax(trainRInput, oldTrainRInput - maxDec * 20.0);

    // Get current target velocity for next iteration
    oldTrainLInput = adjustedTrainLInput;
    oldTrainRInput = adjustedTrainRInput;

    // Input curved to reduce sensitivity at low speeds
    int trainLrpm = 0.8 *(0.5 * adjustedTrainLInput * fabs(adjustedTrainLInput) + 0.5 * adjustedTrainLInput) * 600;
    int trainRrpm = 0.8 * (0.5 * adjustedTrainRInput * fabs(adjustedTrainRInput) + 0.5 * adjustedTrainRInput) * 600;

    // Set lift to coast if the encoder can tell the lift is near the bottom, to avoid motor burnout
    // Absolute value so team can move the encoder without worrying about orientation
    if (fabs(ArmEncoder.position(deg)) > 10 || !ArmEncoder.installed()) {
      Lift.setStopping(hold);
    } else {
      Lift.setStopping(coast);
    }

    DrivetrainL.spin(fwd, trainLrpm, rpm);
    DrivetrainR.spin(fwd, trainRrpm, rpm);

    Claw.spin(fwd, (Controller1.ButtonR2.pressing() - Controller1.ButtonR1.pressing()) * 200, rpm);
    Lift.spin(fwd, (Controller1.ButtonL1.pressing() - 0.5 * Controller1.ButtonL2.pressing()) * 200, rpm);

    this_thread::sleep_for(20);
  }
}

int main() {
  // Set up callbacks 
  Competition.autonomous(autonomous);
  Competition.drivercontrol(drivercontrol);

  pre_auton();

  // Prevent main from exiting
  while (true) {
    this_thread::sleep_for(100);
  }
}
