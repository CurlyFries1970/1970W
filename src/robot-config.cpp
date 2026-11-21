#include "vex.h"

using namespace vex;

brain Brain;
competition Competition;

motor TopL = motor(PORT16, ratio6_1, false);
motor BottomL = motor(PORT18, ratio6_1, false);
motor TopR = motor(PORT14, ratio6_1, true);
motor BottomR = motor(PORT15, ratio6_1, true);
motor Claw = motor(PORT11, ratio18_1, true);
motor Flipper = motor(PORT5, ratio18_1, false);
motor LiftL = motor(PORT6, ratio18_1, true);
motor LiftR = motor(PORT7, ratio18_1, false);

motor_group DrivetrainL = motor_group(TopL, BottomL);
motor_group DrivetrainR = motor_group(TopR, BottomR);
motor_group Lift = motor_group(LiftL, LiftR);

rotation ArmEncoder = rotation(PORT19, false);

controller Controller1 = controller(primary);