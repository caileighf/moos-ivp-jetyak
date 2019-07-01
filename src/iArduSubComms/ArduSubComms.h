/************************************************************/
/*    NAME: Muthukumaran Chandrasekaran                     */
/*    ORGN: MIT Cambridge MA                                */
/*    FILE: ArduSubComms.h                                  */
/*    DATE: October 13th, 2017                              */
/************************************************************/

#ifndef ArduSubComms_HEADER
#define ArduSubComms_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "mavlink.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;

class UDPClient{
   public:
      //UDPClient();
      UDPClient(
         boost::asio::io_service& io_service, 
         const std::string& host, 
         const std::string& port
      ) : io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), 0)) {
         udp::resolver resolver(io_service_);
         udp::resolver::query query(udp::v4(), host, port);
         udp::resolver::iterator iter = resolver.resolve(query);
         endpoint_ = *iter;
      }

      ~UDPClient()
      {
         socket_.close();
      }

      void send(const mavlink_message_t* msg) {
         socket_.send_to(boost::asio::buffer((void*)msg, sizeof(mavlink_message_t)), endpoint_);
      }

   private:
      boost::asio::io_service& io_service_;
      udp::socket socket_;
      udp::endpoint endpoint_;
};

class ArduSubComms : public AppCastingMOOSApp
{
 public:
   ArduSubComms();
   ~ArduSubComms();

 protected: // Standard MOOSApp functions to overload
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
   bool buildReport();

 protected:
   void registerVariables();
   bool isGoodSerialComms();
   bool write(uint8_t *val, uint16_t &len);
   bool write(mavlink_message_t &msg, uint16_t &len);
   bool onData(const boost::system::error_code& e, std::size_t size);
   bool startReceive();
   bool connectSerial(const std::string& port_name, uint16_t baud);

 private: // Configuration variables
   uint64_t mav_msg_tx_count;
   uint64_t mav_msg_rx_count;
   bool     m_using_companion_comp;
   bool     m_good_serial_comms;
 private: // State variables
   mavlink_message_t                            m_mavlink_msg;
 
   // common for UDP & Serial
   boost::asio::io_service                      m_io;
   boost::thread                                m_runner;
   boost::shared_ptr<boost::system::error_code> m_error;

   // UDP comms related 
   std::string                                  m_mavlink_host;
   boost::shared_ptr<udp::socket>               m_udp;
   UDPClient                                  * m_udp_client;
 
   // Serial comms related 
   std::string                                  m_mavlink_port;
   unsigned int                                 m_mavlink_baud;
   boost::shared_ptr<boost::asio::serial_port>  m_serial;
   boost::asio::serial_port                     m_serial_port;
   boost::asio::streambuf                       m_buffer; // for data coming IN

   // write handler - needed for async_write calls - SERIAL COMMS
   struct write_handler {
      void operator()(const boost::system::error_code& ec, std::size_t bytes_transferred) {}
   } handler;
};

#endif
