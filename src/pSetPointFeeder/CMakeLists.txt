#--------------------------------------------------------
# The CMakeLists.txt for:                       pSetPointFeeder
# Author(s):                              Caileigh Fitzgerald
#--------------------------------------------------------

SET(SRC
  SetPointFeeder.cpp
  SetPointFeeder_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pSetPointFeeder ${SRC})

TARGET_LINK_LIBRARIES(pSetPointFeeder
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

