#--------------------------------------------------------
# The CMakeLists.txt for:                       iArduSubComms
# Author(s):                              Muthukumaran Chandrasekaran
#--------------------------------------------------------
INCLUDE_DIRECTORIES(../../thirdparty/common)

find_package(Boost COMPONENTS thread system REQUIRED)

SET(SRC
  ArduSubComms.cpp
  ArduSubComms_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iArduSubComms ${SRC})
include_directories(${Boost_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(iArduSubComms
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread
   ${Boost_THREAD_LIBRARY}
   ${Boost_SYSTEM_LIBRARY})

