#--------------------------------------------------------
# The CMakeLists.txt for:                       iArduSubComms
# Author(s):                              Muthukumaran Chandrasekaran
#--------------------------------------------------------
INCLUDE_DIRECTORIES(../../thirdparty/common)

SET(SRC
  ArduSubComms.cpp
  ArduSubComms_Info.cpp
  main.cpp
)

include_directories(${boost_INCLUDE_DIR})

ADD_EXECUTABLE(iArduSubComms ${SRC})

TARGET_LINK_LIBRARIES(iArduSubComms
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

