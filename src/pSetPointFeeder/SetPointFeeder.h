/************************************************************/
/*    NAME: Caileigh Fitzgerald                                              */
/*    ORGN: MIT                                             */
/*    FILE: SetPointFeeder.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef SetPointFeeder_HEADER
#define SetPointFeeder_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <iostream>
#include <vector>
#include <sstream>

struct NAV_MSG
{
    // These are the only values we can change in pMavlink
    double x;
    double y;
    double z;
    double yaw;
    std::stringstream ss;

    std::string json() {
        ss.str(std::string()); // clear ss obj
        ss << "{x:" << x << 
              ",y:" << y << 
              ",z:" << z << 
              ",yaw:" << yaw << "}";
        return(ss.str());
    }
};

class SetPointFeeder : public AppCastingMOOSApp
{
 public:
   SetPointFeeder();
   ~SetPointFeeder() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   void sendNullSetPoint();

 private: // Configuration variables

 private: // State variables
    double   mValidXDelta;
    double   mValidYDelta;
    double   mValidZDelta;
    double   mValidYawDelta;
    bool     mIsOffBoardMode;
    
    std::vector<double>   mDesiredSpeed;
    std::vector<double>   mDesiredHeading;
    std::vector<NAV_MSG>    mNavMsgs;
};

#endif 
