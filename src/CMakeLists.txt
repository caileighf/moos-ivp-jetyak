##############################################################################
# FILE:        moos-ivp-bluerov/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-bluerov source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
# ADD_SUBDIRECTORY(pMavlinkConverter)
# ADD_SUBDIRECTORY(iArduSubComms)
# ADD_SUBDIRECTORY(pSetPointFeeder)
ADD_SUBDIRECTORY(iMavlinkOffboardController)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

