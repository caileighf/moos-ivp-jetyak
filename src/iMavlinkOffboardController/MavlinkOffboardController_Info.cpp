/****************************************************************/
/*   NAME: Caileigh Fitzgerald                                             */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: MavlinkOffboardController_Info.cpp                               */
/*   DATE: Dec 29th 1963                                        */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "MavlinkOffboardController_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The iMavlinkOffboardController application is used for               ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: iMavlinkOffboardController file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch iMavlinkOffboardController with the given process name         ");
  blk("      rather than iMavlinkOffboardController.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of iMavlinkOffboardController.        ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iMavlinkOffboardController Example MOOS Configuration                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = iMavlinkOffboardController                              ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  * for a serial connection between companion pc and pixhawk *  ");
  blk("  CONN_URL = \"serial:///dev/ttyACM0:115200\"                   ");
  blk("                                                                ");
  blk("  * for a UDP connection between companion pc and pixhawk *  ");
  blk("  CONN_URL = \"udp://:\"                   ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iMavlinkOffboardController INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_MESSAGE = src_node=alpha,dest_node=bravo,var_name=FOO,   ");
  blk("                 string_val=BAR                                 ");
  blk("                                                                ");
  blk("  DESIRED_SPEED, DESIRED_HEADING                                ");
  blk("  START_OFFBOARD_MODE, STOP_OFFBOARD_MODE                       ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  MAVLINK_RX_TELEMETRY_REPORT                                   ");
  blk("  MAVLINK_RX_TELEMETRY_EULER                                    ");
  blk("  MAVLINK_RX_TELEMETRY_POSITION                                 ");
  blk("  MAVLINK_RX_TELEMETRY_*                                        ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("iMavlinkOffboardController", "gpl");
  exit(0);
}

