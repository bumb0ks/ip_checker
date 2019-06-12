#ifndef PINGER
#define PINGER

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <istream>
#include <iostream>
#include <ostream>
#include <string>

#include "icmp_header.hpp"
#include "ipv4_header.hpp"

using boost::asio::ip::icmp;
using boost::asio::steady_timer;

/*
    Class contains all async functionality required to ping ip address.
*/

class pinger
{
public:
  pinger(boost::asio::io_context& io_context,
    boost::function<void (std::string)> callback);
  pinger(pinger&& p);

  ~pinger();

  void process(const std::string& destination);

  bool is_busy() const;

private:
  void start_send();

  void handle_timeout();

  void start_receive();

  void handle_receive(std::size_t length);

  unsigned short get_identifier()
  {
    return m_id;
  }

  void generate_new_id()
  {
    m_id = rand() % 65500;
  }

  icmp::resolver resolver_;
  icmp::endpoint destination_;
  icmp::socket socket_;
  steady_timer timer_;
  unsigned short sequence_number_;
  boost::asio::chrono::steady_clock::time_point time_sent_;
  boost::asio::streambuf reply_buffer_;
  std::size_t num_replies_;
  std::string destination_ip;
  boost::function<void (std::string)> m_callback;

  bool timed_out;
  bool m_busy;

  unsigned short m_id = 0;
};

#endif