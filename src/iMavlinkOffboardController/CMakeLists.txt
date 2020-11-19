#--------------------------------------------------------
# The CMakeLists.txt for:     iMavlinkOffboardController
# Author(s):                  Caileigh Fitzgerald
#--------------------------------------------------------
cmake_minimum_required(VERSION 2.8.12)
find_package(MAVSDK REQUIRED)

add_definitions("-std=c++11")

SET(SRC
  MavlinkOffboardController.cpp
  MavlinkOffboardController_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iMavlinkOffboardController ${SRC})

TARGET_LINK_LIBRARIES(iMavlinkOffboardController
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread
   MAVSDK::mavsdk_action
   MAVSDK::mavsdk_offboard
   MAVSDK::mavsdk_telemetry
   MAVSDK::mavsdk)

