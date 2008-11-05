#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "cwf/fcgi_spec.hpp"
#include "cwf/cwf.h"

namespace cwf {

void Connection::Start() {
  socket_.async_read_some(boost::asio::buffer(buffer_),
    strand_.wrap(
    boost::bind(&Connection::HandleRead, shared_from_this(),
    boost::asio::placeholders::error,
    boost::asio::placeholders::bytes_transferred)));
}

bool Handler::ProcessHeader(const fcgi::Header &header) {
  // 1 check version
  // 2 request_id == FCGI_NULL_REQUEST_ID, get param
  // 3 request_id = FCGI_BEGIN_REQUEST, begin request
	
	if (header.version() != fcgi::VERSION_1){
		return false; // "version-not-supported";
	}

	int type = header.type();
	switch (type){
	case fcgi::BEGIN_REQUEST:
		break;
	case fcgi::PARAMS:
		break;
	case fcgi::STDIN:
	default:
		break;
	}
	
	int request_id = header.request_id();
	int content_length = header.content_length();


	// from fcgi devkit
	/*
    int requestId;
    if(header.version != FCGI_VERSION_1) {
        return FCGX_UNSUPPORTED_VERSION;
    }
    requestId =        (header.requestIdB1 << 8)
        + header.requestIdB0;
    data->contentLen = (header.contentLengthB1 << 8)
        + header.contentLengthB0;
    data->paddingLen = header.paddingLength;
    if(header.type == FCGI_BEGIN_REQUEST) {
        return ProcessBeginRecord(requestId, stream);
    }
    if(requestId  == FCGI_NULL_REQUEST_ID) {
        return ProcessManagementRecord(header.type, stream);
    }
    if(requestId != data->reqDataPtr->requestId) {
        return SKIP;
    }
    if(header.type != data->type) {
        return FCGX_PROTOCOL_ERROR;
    }
    return STREAM_RECORD;
	*/
}


void Connection::HandleRead(const boost::system::error_code& e,
														std::size_t bytes_transferred) {
	// If an error occurs then no new asynchronous operations are started. This
	// means that all shared_ptr references to the Connection object will
	// disappear and the object will be destroyed automatically after this
	// handler returns. The Connection class's destructor closes the socket.
	if (e)
		return;

  Reply reply;
  boost::tribool result;
  boost::tie(result, boost::tuples::ignore) = request_handler_.Parse(
    request_, buffer_.data(), bytes_transferred, reply);

#if 0
	readed_ += bytes_transferred;

	if (!got_header_) {
		// Parse
		if (readed_ < sizeof(fcgi::Header))
			; // read more

		fcgi::Header *h =  reinterpret_cast<fcgi::Header *>(buffer_.data());
		// bytes_transferred						// 
		got_header_ = ProcessHeader(*h);

    if (!got_header_) {
      // write not support?
    } else {
      // 
    }
	}
  // SCRIPT_FILENAME
#endif

#if 0	
  if (!e) {
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if (result)
    {
      request_handler_.handle_request(request_, reply_);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          strand_.wrap(
            boost::bind(&Connection::HandleWrite, shared_from_this(),
              boost::asio::placeholders::error)));
    }
    else if (!result)
    {
      reply_ = reply::stock_reply(reply::bad_request);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          strand_.wrap(
            boost::bind(&Connection::HandleWrite, shared_from_this(),
              boost::asio::placeholders::error)));
    }
    else
    {
      socket_.async_read_some(boost::asio::buffer(buffer_),
          strand_.wrap(
            boost::bind(&Connection::HandleRead, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred)));
    }
  }
#endif
}

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
