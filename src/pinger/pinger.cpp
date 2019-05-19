#include "pinger.h"


namespace chrono = boost::asio::chrono;
  
pinger::pinger(boost::asio::io_context& io_context,
               boost::function<void (std::string)> callback)
    : resolver_(io_context), socket_(io_context, icmp::v4()),
        timer_(io_context), sequence_number_(5), num_replies_(0),
        m_busy(false), m_callback(callback)
{
}

pinger::pinger(pinger&& p) : 
    resolver_(std::move(p.resolver_)),
    socket_(std::move(p.socket_)),
    timer_(std::move(p.timer_)), 
    sequence_number_(std::move(p.sequence_number_)), 
    num_replies_(0),
    m_busy(std::move(p.m_busy)),
    m_callback(std::move(p.m_callback))
{
}

pinger::~pinger()
{
    socket_.close();
}

void pinger::process(const std::string& destination)
{
    m_busy = true;
    destination_ip = destination;
    
    destination_ = *resolver_.resolve(icmp::v4(), destination_ip, "").begin();

    timed_out = false;

    start_send();
    start_receive();
}

bool pinger::is_busy() const
{
    return m_busy;
}

void pinger::start_send()
{
    std::string body("\"Hello!\" from Asio ping.");

    icmp_header echo_request;
    echo_request.type(icmp_header::echo_request);
    echo_request.code(0);
    echo_request.identifier(get_identifier());
    echo_request.sequence_number(sequence_number_);
    compute_checksum(echo_request, body.begin(), body.end());

    boost::asio::streambuf request_buffer;
    std::ostream os(&request_buffer);
    os << echo_request << body;

    time_sent_ = steady_timer::clock_type::now();
    socket_.send_to(request_buffer.data(), destination_);

    num_replies_ = 0;
    timer_.expires_at(time_sent_ + chrono::milliseconds(1000));
    timer_.async_wait(boost::bind(&pinger::handle_timeout, this));
}

void pinger::handle_timeout()
{
    timed_out = true;
    socket_.cancel();
    m_busy = false;
    if (num_replies_ != 0)
    {
        m_callback(destination_ip + "-1");
    }
    else
        m_callback(destination_ip + "-0");

}

void pinger::start_receive()
{
    if (timed_out) return;
    reply_buffer_.consume(reply_buffer_.size());
    
    socket_.async_receive(reply_buffer_.prepare(65536),
        boost::bind(&pinger::handle_receive, this, _2));
}

void pinger::handle_receive(std::size_t length)
{
    reply_buffer_.commit(length);

    std::istream is(&reply_buffer_);
    ipv4_header ipv4_hdr;
    icmp_header icmp_hdr;
    is >> ipv4_hdr >> icmp_hdr;

    if (is && icmp_hdr.type() == icmp_header::echo_reply
            // && icmp_hdr.identifier() == get_identifier()
            && icmp_hdr.sequence_number() == sequence_number_)
    {
        num_replies_++;
    }
    timer_.cancel();
}