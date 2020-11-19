/************************************************************/
/*    NAME: Caileigh Fitzgerald                                              */
/*    ORGN: MIT                                             */
/*    FILE: MavlinkOffboardController.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MavlinkOffboardController.h"

using namespace std;
using namespace mavsdk;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

// Handles Action's result
inline void action_error_exit(Action::Result result, const std::string& message)
{
    if (result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        // exit(EXIT_FAILURE);
    }
}

// Handles Offboard's result
inline void offboard_error_exit(Offboard::Result result, const std::string& message)
{
    if (result != Offboard::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        // exit(EXIT_FAILURE);
    }
}

// Handles connection result
inline void connection_error_exit(ConnectionResult result, const std::string& message)
{
    if (result != ConnectionResult::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        // exit(EXIT_FAILURE);
    }
}

// Logs during Offboard control
inline void offboard_log(const std::string& offb_mode, const std::string msg)
{
    std::cout << "[" << offb_mode << "] " << msg << std::endl;
}

void wait_until_discover(Mavsdk& mavsdk)
{
    std::cout << "Waiting to discover system..." << std::endl;
    std::promise<void> discover_promise;
    auto discover_future = discover_promise.get_future();

    mavsdk.subscribe_on_new_system([&mavsdk, &discover_promise]() {
        const auto system = mavsdk.systems().at(0);

        if (system->is_connected()) {
            std::cout << "Discovered system" << std::endl;
            discover_promise.set_value();
        }
    });

    discover_future.wait();
}

//---------------------------------------------------------
// 
bool MavlinkOffboardController::connectToPixhawk(bool wait)
{
  m_connection_result = m_mavsdk.add_any_connection(m_connection_url);

  bool is_connected;
  if (m_connection_result != ConnectionResult::Success) 
  {
    std::cout << ERROR_CONSOLE_TEXT << "Connection failed: " << m_connection_result
              << NORMAL_CONSOLE_TEXT << std::endl;

    is_connected = false;
  }
  else 
  {
    is_connected = true;
    if (wait)
    {
      // Wait for the system to connect via heartbeat
      wait_until_discover(m_mavsdk); 
    }
  }
  
  return(is_connected);
}

//---------------------------------------------------------
// setupPixhawk()

bool MavlinkOffboardController::setupPixhawk()
{
  return(setupPixhawk(m_connection_url));
}

bool MavlinkOffboardController::setupPixhawk(string connection_url)
{
  //
  //  TODO: add exception handling
  //
  m_connection_url = connection_url;
  bool wait_until_discover = true;

  if (connectToPixhawk(wait_until_discover))
  {
    m_mav_sys_ok = true;
    // System got discovered.
    m_system = m_mavsdk.systems().at(0);
    m_action = std::make_shared<Action>(m_system);
    m_offboard = std::make_shared<Offboard>(m_system);
    m_telemetry = std::make_shared<Telemetry>(m_system);

    while (!m_telemetry->health_all_ok()) {
        std::cout << "Waiting for system to be ready" << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "System is ready" << std::endl;
    return(true);
  }
  return(false);
}

//---------------------------------------------------------
//
bool MavlinkOffboardController::startOffboardMode()
{
  if (isPixhawkConnected() && !isOffboardActive())
  {
    // First arm the pixhawk -- this is required for offboard mode
    Action::Result arm_result = m_action->arm();
    action_error_exit(arm_result, "Arming failed");
    std::cout << "Armed" << std::endl;

    // send offboard message BEFORE staring offboard mode
    m_offboard->set_velocity_body(m_stay);

    // start offboard mode
    Offboard::Result offboard_result = m_offboard->start();
    offboard_error_exit(offboard_result, "Offboard start failed: ");
    offboard_log(m_offb_mode, "Offboard started");

    // send another message "null" msg
    m_offboard->set_velocity_body(m_stay);
    return(true);
  }
  return(false);
}

//----------------------------------------------------------
//
bool MavlinkOffboardController::stopOffboardMode()
{
  if (isPixhawkConnected() && isOffboardActive())
  {
    Offboard::Result offboard_result = m_offboard->stop();
    offboard_error_exit(offboard_result, "Offboard stop failed: ");
    offboard_log(m_offb_mode, "Offboard stopped");

    Action::Result arm_result = m_action->disarm();
    action_error_exit(arm_result, "Disarming failed");
    std::cout << "Disarmed" << std::endl;

    return(true);
  }
  return(false);
}

bool MavlinkOffboardController::isOffboardActive()
{
  if (isPixhawkConnected())
    return(m_offboard->is_active());
  return(m_mav_sys_ok);
}

bool MavlinkOffboardController::isPixhawkConnected()
{
  if (m_mav_sys_ok)
    return(m_system->is_connected());
  return(m_mav_sys_ok);
}

//----------------------------------------------------------
//
bool MavlinkOffboardController::sendOffboardCommand()
{
  // when offboard mode is active it needs to receive messages at a rate >2Hz
  // .. or it will stop
  if (isOffboardActive())
  {
    m_offboard->set_velocity_body(m_current_offboard_cmd);

    // update prev cmd to current -- TODO check attr for equality to avoid 
    //                               .. re-assigning values that are the same
    m_previous_offboard_cmd.forward_m_s = m_current_offboard_cmd.forward_m_s;
    m_previous_offboard_cmd.yawspeed_deg_s = m_current_offboard_cmd.yawspeed_deg_s;
    
    return(true);
  }
  return(false);
}

//---------------------------------------------------------
//
void MavlinkOffboardController::pubTelemetryReport()
{
  if (isPixhawkConnected())
  {
    // for options see: 
    // https://github.com/mavlink/MAVSDK/blob/develop/src/plugins/telemetry/telemetry.cpp
    std::stringstream ss;
    ss << m_telemetry->status_text();
    Notify("MAVLINK_RX_TELEMETRY_REPORT", ss.str());

    ss.str(std::string());
    ss << m_telemetry->attitude_euler();
    Notify("MAVLINK_RX_TELEMETRY_EULER", ss.str());

    ss.str(std::string());
    ss << m_telemetry->position();
    Notify("MAVLINK_RX_TELEMETRY_POSITION", ss.str());
  }
}

//---------------------------------------------------------
// Constructor

MavlinkOffboardController::MavlinkOffboardController()
{
  m_connection_url = "invalid";
  m_serial_port = "invalid";
  m_baudrate = "invalid";
  m_conn_mode = "serial";
  m_offb_mode = "BODY";
  m_mav_sys_ok = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MavlinkOffboardController::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    double dval   = msg.GetDouble();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if (key == "DESIRED_SPEED") 
     {
      // only the most recent speed will be saved
      m_current_offboard_cmd.forward_m_s = (float) dval;
     }
     else if (key == "DESIRED_HEADING")
     {
      // only the most recent heading will be saved
      m_current_offboard_cmd.yawspeed_deg_s = (float) dval;
     }
     else if (key == "START_OFFBOARD_MODE")
     {
      if (isPixhawkConnected())
      {
        if (startOffboardMode() && isOffboardActive())
          reportEvent("Offboard mode started");
        else
          reportRunWarning("Tried to start offboard mode and failed!");
      }
      else
        // TODO: should be an event? how do we want to handle this? try to connect again?
        reportRunWarning("Cannot start offboard mode without being connected to the Pixhawk!");
     }
     else if (key == "STOP_OFFBOARD_MODE")
     {
      if (isOffboardActive())
      {
        if (stopOffboardMode() && !isOffboardActive())
          reportEvent("Offboard mode stopped");
        else
          reportRunWarning("Tried to stop offboard mode and failed!");
      }
      else
        reportEvent("Offboard mode must be running before you can stop it!");
     }

     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   // if (!isPixhawkConnected())
   // {
   //  // re-try connecting
   //  if (setupPixhawk())
   //    reportEvent("Successfully connected to the Pixhawk!");
   //  else
   //    reportEvent("FAILED to connect to the Pixhawk!");
   // }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool MavlinkOffboardController::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MavlinkOffboardController::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // send most recent heading & speed command (if any)
  sendOffboardCommand();
  // publish telemetry report
  pubTelemetryReport();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MavlinkOffboardController::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if (param == "PORT") 
    {
      m_serial_port = value;
      handled = true;
    }
    else if (param == "BAUD")
    {
      m_baudrate = value;
      handled = true;
    }
    else if (param == "MODE")
    {
      m_conn_mode = value;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  //
  //  Build connection URL string and setup Pixhawk
  //    TODO: currently mode defaults to serial. Needs logic for socket comms eventually 
  //
  if (m_serial_port != "invalid" && m_baudrate != "invalid")
  {
    if (setupPixhawk((m_conn_mode + "://" + m_serial_port + ":" + m_baudrate)))
      m_mav_sys_ok = true;
    else
      reportRunWarning("Unable to connect to the Pixhawk! Check that port/baud are correct\n\tURL: " + m_connection_url + "\n");
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void MavlinkOffboardController::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DESIRED_SPEED", 0);
  Register("DESIRED_HEADING", 0);
  Register("START_OFFBOARD_MODE", 0);
  Register("STOP_OFFBOARD_MODE", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MavlinkOffboardController::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << " iMavlinkOffboardController                  \n";
  m_msgs << "============================================ \n";

  m_msgs << " Connected to Pixhawk: " << boolalpha << isPixhawkConnected() << endl;
  m_msgs << "     In offboard mode: " << boolalpha << isOffboardActive() << endl << endl;

  m_msgs << "       Connection URL: " << m_connection_url << endl;

  return(true);
}




