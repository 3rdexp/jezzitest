#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "cwf/fcgi_spec.hpp"
#include "cwf/cwf.h"

#ifndef Assert
#define Assert(x) if (!(x)) __asm int 3;
#endif

namespace cwf {

struct NestedStream {
  NestedStream(const char *stream, std::size_t length) 
    : stream_(stream), length_(length), next_(stream) 
  {}  

  int ReadChar() {
    if (next_ != stream_ + length_)
      return *next_++;

    return EOF;
  }

  int ReadBuffer(char *buffer, int buffer_length) {
    if (next_ + buffer_length <= stream_ + length_) {
      memcpy(buffer, next_, buffer_length);
      next_ += buffer_length;
      return buffer_length;
    }

    // read more?
    return 0;
  }

  int ReadBuffer(unsigned char *buffer, int buffer_length) {
    return ReadBuffer((char*)buffer, buffer_length);
  }

private:
  const char *stream_;
  std::size_t length_;
  const char *next_;
};

bool Parser::Process(const char *stream, std::size_t length, Request &request) {
  using namespace fcgi;

  if (length < sizeof(BeginRequestRecord))
    return false;

  const BeginRequestRecord *brr =  reinterpret_cast<const BeginRequestRecord *>(stream);
  
  boost::uint16_t request_id = brr->header_.request_id();
  if (brr->header_.version() < VERSION_1)
    return false;

  stream += sizeof BeginRequestRecord;
  length -= sizeof BeginRequestRecord;
  if (length < sizeof Header)
    return false;
  const Header * header = reinterpret_cast<const Header *>(stream);

  if (header->type() != PARAMS)
    return false;

  NestedStream ns(stream + sizeof(Header),
      header->content_length());
  
  // name-value pair
  // translate from fcgiapp.c
  unsigned char lens[3] = {0};
  int name_len = 0;
  while ((name_len = ns.ReadChar()) != EOF) {
    if ((name_len & 0x80) !=0 ) {
      if (3 != ns.ReadBuffer(lens, 3))
        return false; // read params error

      name_len = ((name_len & 0x7f) << 24) + (lens[0] << 16)
        + (lens[1] << 8) + lens[2];
    }
    Assert(name_len < 0xffff);
    if (name_len == 0)
      break;

    int value_len = ns.ReadChar();
    if (value_len == EOF)
      return false;

    if ((value_len & 0x80) != 0) {
      if (3 != ns.ReadBuffer(lens, 3))
        return false;

      value_len = ((value_len & 0x7f) << 24) + (lens[0] << 16)
        + (lens[1] << 8) + lens[2];
    }
    Assert(value_len < 0xffff);

    std::string name(name_len, '\0');
    if (name_len != ns.ReadBuffer(&name[0], name_len))
      return false;

    std::string value;
    if (value_len) {
      value.swap(std::string(value_len, '\0'));
      if (value_len != ns.ReadBuffer(&value[0], value_len))
        return false;
    }

    request.PutParam(name, value);
  }
  return true;
}

// map
// 
bool Handle::Render(const Request &req, Reply &reply) {
  return true;
}

std::vector<boost::asio::const_buffer> Reply::to_buffers() const {
  std::vector<boost::asio::const_buffer> buffers;
  buffers.push_back(boost::asio::const_buffer(&header_, sizeof(header_)));
  // buffers.push_back(boost::asio::const_buffer(&buffer_[0], buffer_.size()));
  return buffers;
}

void Connection::Start() {
  socket_.async_read_some(boost::asio::buffer(buffer_),
    strand_.wrap(
    boost::bind(&Connection::HandleRead, shared_from_this(),
    boost::asio::placeholders::error,
    boost::asio::placeholders::bytes_transferred)));
}

void Connection::HandleRead(const boost::system::error_code& e,
														std::size_t bytes_transferred) {
	// If an error occurs then no new asynchronous operations are started. This
	// means that all shared_ptr references to the Connection object will
	// disappear and the object will be destroyed automatically after this
	// handler returns. The Connection class's destructor closes the socket.
	if (e)
		return;
  
  boost::tribool result;
  boost::tie(result, boost::tuples::ignore) = parser_.Parse(
    buffer_.data(), bytes_transferred, request_);

  if (result) {
    reply_ = Reply(fcgi::STDOUT, request_.request_id());
    if (request_handler_.Render(request_, reply_))
      boost::asio::async_write(socket_, reply_.to_buffers(),
        strand_.wrap(
          boost::bind(&Connection::HandleWrite, shared_from_this(),
          boost::asio::placeholders::error)));

    
  }
  else if (!result) {
    // bad_request
  } else {
    // do nothing
  }

  // read more
}

void Connection::HandleWrite(const boost::system::error_code& e) {
  if (!e) {
    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }

  // No new asynchronous operations are started. This means that all shared_ptr
  // references to the connection object will disappear and the object will be
  // destroyed automatically after this handler returns. The connection class's
  // destructor closes the socket.
}

//////////////////////////////////////////////////////////////////////////
//
bool Server::Init() {
  new_connection_.reset(new Connection(io_service_, request_handler_));

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address_, port_);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  acceptor_.async_accept(new_connection_->socket(),
    boost::bind(&Server::HandleAccept, this,
    boost::asio::placeholders::error));
  return true;
}

void Server::Run() {
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
      boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void Server::Stop() {
  io_service_.stop();
}

void Server::HandleAccept(const boost::system::error_code& e) {
  if (!e) {
    new_connection_->Start();
    new_connection_.reset(new Connection(io_service_, request_handler_));
    acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&Server::HandleAccept, this,
      boost::asio::placeholders::error));
  }
}

}
