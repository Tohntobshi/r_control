#include "controlsReceiver.h"
#include "robotController.h"

int main()
{
  RobotController robot_controller;
  ControlsReceiver controls_reciever([&] (char command) -> void {
    robot_controller.command(command);
  });
  controls_reciever.registerService();
  controls_reciever.startListening();
  return 0;
}