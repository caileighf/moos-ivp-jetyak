/************************************************************/
/*    NAME: Muthukumaran Chandrasekaran                     */
/*    ORGN: MIT Cambridge MA                                */
/*    FILE: ArduSubComms.cpp                                */
/*    DATE: October 13th, 2017                              */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "ArduSubComms.h"

// Max rate to receive MOOS updates (0 indicates no rate, get all)
#define DEFAULT_REGISTER_RATE 0.0

// Enable/disable debug code
#define DEBUG 1

// TOGGLE_PORT 1 for Enabling Comms via UDP (for BlueROV2 control (or in SITL) from MOOS running on GCS)
// TOGGLE_PORT 0 for Enabling Comms via SERIAL Port (for BlueROV2 control from MOOS running on companion computer onboard)
#define TOGGLE_PORT 0
bool debug = true;

using namespace std;

//---------------------------------------------------------
// Constructor

ArduSubComms::ArduSubComms() : m_serial_port(m_io) 
{
  if (debug) cout << "In ArduSubComms Constructor!" << endl;

  mav_msg_tx_count = 0;
  mav_msg_rx_count = 0;

  m_using_companion_comp = false;
  m_good_serial_comms = false;

  if(TOGGLE_PORT){
    m_mavlink_host = "127.0.0.1"; // this can be updated in mission file during launch
    m_mavlink_port = "14000";
  }else{
    m_mavlink_port = "/dev/ttyAMA0";
    m_mavlink_baud = 115200;
  }
}

//---------------------------------------------------------
// Destructor

ArduSubComms::~ArduSubComms()
{
  m_serial_port.close();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ArduSubComms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

    if(p->IsName("MAVLINK_MSG_SET_POSITION_TARGET")) 
    {
      memcpy((void*)&m_mavlink_msg, (void*)p->GetBinaryData(), p->GetBinaryDataSize());

      if(!m_using_companion_comp){
        m_udp_client->send(&m_mavlink_msg);

        mav_msg_tx_count++;
      }else{
        uint16_t len = p->GetBinaryDataSize();
        // write to the serial port
        bool sucessful_write = write(m_mavlink_msg, len);

        if (sucessful_write) mav_msg_tx_count++;
      }


      //debug of mavlink_message_t for confirmation
      /*********************************************/
      if (DEBUG){
        Notify("VERIFY_MSG",(void*)&m_mavlink_msg, p->GetBinaryDataSize());
      }
      /***********************************************/

    }
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportConfigWarning("Unhandled Mail: " + key);

    #if 0 // Keep these around just for template
      string comm  = msg.GetCommunity();
      double dval  = msg.GetDouble();
      string sval  = msg.GetString();
      string msrc  = msg.GetSource();
      double mtime = msg.GetTime();
      bool   mdbl  = msg.IsDouble();
      bool   mstr  = msg.IsString();
    #endif

    
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ArduSubComms::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool ArduSubComms::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool ArduSubComms::OnStartUp()
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

    if(param == "PORT") {
      m_mavlink_port = value;
      handled = true;
    }
    else if(param == "BAUD") {
      m_mavlink_baud = atoi(value.c_str());
      handled = true;
    }
    else if(param == "USING_COMPANION_COMPUTER") {
      m_using_companion_comp = (value == "true" ? true : false);
      handled = true;
    }
    else if (param == "APPTICK" || param == "COMMSTICK")
      handled = true; 

    if (!handled)
      reportConfigWarning("Unhandled Configuration param: " + param);

  }

  if(m_using_companion_comp) // if using companion computer then connect via hardware serial
  {
    if (debug) cout << "inside block to open up hardware serial port!" << endl;
    m_good_serial_comms = connectSerial(m_mavlink_port, m_mavlink_baud);
    if (debug) cout << "serial " << (m_good_serial_comms ? "HAS" : "HAS NOT") << " connected" << endl;

    //m_serial = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(m_io, m_mavlink_port));
    //m_serial->set_option(boost::asio::serial_port_base::baud_rate(m_mavlink_baud));
  }
  else
  {
    m_udp_client = new UDPClient(m_io, m_mavlink_host, m_mavlink_port);
  }

  bool isAppCasting = buildReport();
  if (!isAppCasting)
    reportConfigWarning("App is NOT AppCasting!!");

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: isGoodSerialComms()

bool ArduSubComms::isGoodSerialComms()
{
  if (!m_using_companion_comp)
    return false;

  return m_serial_port.is_open();
}

//---------------------------------------------------------
// Procedure: registerVariables

void ArduSubComms::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("MAVLINK_MSG_SET_POSITION_TARGET", DEFAULT_REGISTER_RATE);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool ArduSubComms::buildReport()
{
  AppCastingMOOSApp::buildReport();
  
  m_msgs << "============================================ \n";
  m_msgs << "iArduSubComms                                \n";
  m_msgs << "============================================ \n";
  if (m_using_companion_comp) {
    m_msgs << " PORT: " << m_mavlink_port << " BAUD: " << m_mavlink_baud << endl;
    m_msgs << "\tserial port " << (m_good_serial_comms ? "is" : "is NOT") << " open" << endl;
  }

  ACTable actab(2);
  actab << "mav_msg_TX | mav_msg_RX";
  actab.addHeaderLines();
  actab << (int)mav_msg_tx_count << (int)mav_msg_rx_count;
  m_msgs << actab.getFormattedString();

  return(true);
}

//------------------------------------------------------------
// Procedure: m_connect_serial()

bool ArduSubComms::connectSerial(const std::string& port_name, uint16_t baud)
{
  if (!m_using_companion_comp)
    return false;

  using namespace boost::asio;

  if (!m_serial_port.is_open()) { // if port isn't open try to open
    try {

      if (debug) cout << "in try block in connectSerial()" << endl;

      m_serial_port.open(port_name);
      //Setup port
      m_serial_port.set_option(serial_port::baud_rate(baud));
      m_serial_port.set_option(serial_port::flow_control(serial_port::flow_control::none));
    }
    catch (const boost::system::system_error &e)
    {
      cerr << e.what() << endl;
      cerr << e.code() << endl;
      return false;
    }
  } // end if port not open

  if (m_serial_port.is_open())
  {
    //Start io-service in a background thread.
    //boost::bind binds the ioservice instance
    //with the method call
    m_runner = boost::thread(boost::bind(&boost::asio::io_service::run, &m_io));

    // make sure that the io_service does not return because it thinks it is out of work
    boost::asio::io_service::work work(m_io);
    startReceive();
  }

  return m_serial_port.is_open();
}

//------------------------------------------------------------
// Procedure: startReceive() 

bool ArduSubComms::startReceive()
{
  if (!m_using_companion_comp)
    return false;

  using namespace boost::asio;

  if(!m_serial_port.is_open()) {

    try {
    reportEvent("Reopening serial port");
    m_io.reset(); //debug
    connectSerial(m_mavlink_port, m_mavlink_baud);
    }
    catch (const boost::system::system_error &e)
    {
      cerr << e.what() << endl;
      cerr << e.code() << endl;
      return false;
    }
    return false;
  }
  // Issue an async receive and give it a callback onData that should be called when "\n" is matched, 
  async_read_until(m_serial_port, m_buffer, '\n', boost::bind(&ArduSubComms::onData, this, _1,_2));
  return true;
}

//------------------------------------------------------------
// Procedure: onData() 

bool ArduSubComms::onData(const boost::system::error_code& e, std::size_t size)
{
  if (!m_using_companion_comp)
    return false;

  if (!e) 
  {
    std::istream is(&m_buffer);
    std::string data(size,'\0');
    is.read(&data[0],size);

    if(data[0] == '$') {
      reportEvent("DEBUG CHAR RECOGNIZED data: " + data); 
    }
  }
  else {
    reportRunWarning("received error: " + e.message());
    return false;
  }
  startReceive();
  return false;
}

//------------------------------------------------------------
// Procedure: write() 

bool ArduSubComms::write(mavlink_message_t &msg, uint16_t &len)
{
  if (!m_using_companion_comp)
    return false;

  if (isGoodSerialComms()) {
    try {
    
      boost::asio::async_write(m_serial_port, boost::asio::buffer(&msg, len), handler);
    }
    catch (const boost::system::system_error &e)
    {
      cerr << e.what() << endl;
      cerr << e.code() << endl;
      return false;
    }
    return true;
  }
  else
    reportRunWarning("Bad serial comms. Last write failed!!");
  return false;
}

//------------------------------------------------------------
// Procedure: write() 

bool ArduSubComms::write(uint8_t *val, uint16_t &len)
{
  if (!m_using_companion_comp)
    return false;

  if (isGoodSerialComms()) {
    try {
      boost::asio::async_write(m_serial_port, boost::asio::buffer(val, len), handler);
    }
    catch (const boost::system::system_error &e)
    {
      cerr << e.what() << endl;
      cerr << e.code() << endl;
      return false;
    }
    return true;
  }
  else
    reportRunWarning("Bad serial comms. Last write failed!!");
  return false;
}

