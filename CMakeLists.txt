cmake_minimum_required(VERSION 2.8)
project( RControl )

set(
  TARGETS
    src/main.cpp
    src/robotController.cpp
    src/controlsReceiver.cpp
)


add_executable( rcontrol ${TARGETS} )

target_link_libraries(rcontrol bluetooth)
target_link_libraries(rcontrol wiringPi)
