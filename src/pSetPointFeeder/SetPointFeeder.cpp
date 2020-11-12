/************************************************************/
/*    NAME: Caileigh Fitzgerald                             */
/*    ORGN: MIT                                             */
/*    FILE: SetPointFeeder.cpp                              */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "SetPointFeeder.h"

using namespace std;

//---------------------------------------------------------
// Constructor

SetPointFeeder::SetPointFeeder()
{
  mValidXDelta   = 0.0;
  mValidYDelta   = 0.0;
  mValidZDelta   = 0.0;
  mValidYawDelta = 0.0;

  mIsOffBoardMode = false;
  // mDesiredSpeed;
  // mDesiredHeading;
  mNavMsgs.push_back(NAV_MSG());
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SetPointFeeder::OnNewMail(MOOSMSG_LIST &NewMail)
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
        mDesiredSpeed.push_back(dval);
     }
     else if (key == "DESIRED_HEADING") 
     {
        mDesiredHeading.push_back(dval);
     }

     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: sendNullSetPoint

void SetPointFeeder::sendNullSetPoint()
{
  Notify("DESIRED_SETPOINT", "{\"x\":5.0,\"y\":0.0,\"z\":50.0,\"yaw\":0.0}");
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SetPointFeeder::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SetPointFeeder::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  if (mIsOffBoardMode)
  {

  }
  else 
  {
    // sends zeroed out setpoint.
    // intended to send setpoint messages at apptick
    // .. until we are in "Off Board mode" then we send real data
    sendNullSetPoint(); 
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SetPointFeeder::OnStartUp()
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

    bool handled = true;
    if(param == "FOO") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void SetPointFeeder::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DESIRED_SPEED",0.0);
  Register("DESIRED_HEADING",0.0);
  Register("NAV_X",0.0);
  Register("NAV_Y",0.0);
  Register("NAV_Z",0.0);
  Register("NAV_YAW",0.0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool SetPointFeeder::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << " pSetPointFeeder                             \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "x | y | z | yaw";
  actab.addHeaderLines(); 
  actab << "x" << "y" << "z" << "yaw";
  m_msgs << actab.getFormattedString();

  ACTable actab_bottom(2);
  actab_bottom << "Desired Speed | Desired Heading";
  actab_bottom.addHeaderLines();
  if (!mDesiredSpeed.empty() and !mDesiredHeading.empty());
  m_msgs << actab_bottom.getFormattedString();

  return(true);
}




