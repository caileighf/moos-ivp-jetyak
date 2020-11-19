/************************************************************/
/*    NAME: Caileigh Fitzgerald                                              */
/*    ORGN: MIT                                             */
/*    FILE: MavlinkOffboardController.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef MavlinkOffboardController_HEADER
#define MavlinkOffboardController_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <thread>
#include <sstream>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

class MavlinkOffboardController : public AppCastingMOOSApp
{
 public:
   MavlinkOffboardController();
   ~MavlinkOffboardController() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   bool connectToPixhawk(bool wait);
   bool setupPixhawk(std::string connection_url);
   bool setupPixhawk();
   bool startOffboardMode();
   bool stopOffboardMode();
   bool sendOffboardCommand();
   void pubTelemetryReport();
   bool isOffboardActive();
   bool isPixhawkConnected();

 private: // Configuration variables
    mavsdk::Mavsdk            m_mavsdk;
    std::string               m_connection_url;
    std::string               m_serial_port;
    std::string               m_baudrate;
    std::string               m_conn_mode;
    mavsdk::ConnectionResult  m_connection_result;

    std::shared_ptr<mavsdk::System>    m_system;
    std::shared_ptr<mavsdk::Action>    m_action;
    std::shared_ptr<mavsdk::Offboard>  m_offboard;
    std::shared_ptr<mavsdk::Telemetry> m_telemetry;

    mavsdk::Offboard::VelocityBodyYawspeed m_stay{};
    mavsdk::Offboard::VelocityBodyYawspeed m_current_offboard_cmd{};
    mavsdk::Offboard::VelocityBodyYawspeed m_previous_offboard_cmd{};

    std::string m_offb_mode;
    bool        m_mav_sys_ok;

 private: // State variables
};

#endif 
