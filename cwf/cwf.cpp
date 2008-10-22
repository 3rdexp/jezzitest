#include <boost/bind.hpp>
#include "cwf/cwf.h"

namespace cwf {

void Connection::Start() {
    got_header_ = false;
    socket_.async_read_some(boost::asio::buffer(buffer_),
        strand_.wrap(
        boost::bind(&connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred)));
}

ProcessHeader(FCGI_Header &header) {
    // 1 check version
    // 2 request_id == FCGI_NULL_REQUEST_ID, get param
    // 3 request_id = FCGI_BEGIN_REQUEST, begin request

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
}

void Connection::HandleRead(const boost::system::error_code& e,
        std::size_t bytes_transferred) {

  // If an error occurs then no new asynchronous operations are started. This
  // means that all shared_ptr references to the connection object will
  // disappear and the object will be destroyed automatically after this
  // handler returns. The connection class's destructor closes the socket.
    if (e)
        return;

    if (!got_header_) {
        // Parse
        if (bytes_transferred < sizeof(FCGI_Header))
            ; // read more

        FCGI_Header fcgi_header;
        ProcessHeader(fcgi_header);
    }

  if (!e) {
    boost::tribool result;
    boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if (result)
    {
      request_handler_.handle_request(request_, reply_);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          strand_.wrap(
            boost::bind(&connection::handle_write, shared_from_this(),
              boost::asio::placeholders::error)));
    }
    else if (!result)
    {
      reply_ = reply::stock_reply(reply::bad_request);
      boost::asio::async_write(socket_, reply_.to_buffers(),
          strand_.wrap(
            boost::bind(&connection::handle_write, shared_from_this(),
              boost::asio::placeholders::error)));
    }
    else
    {
      socket_.async_read_some(boost::asio::buffer(buffer_),
          strand_.wrap(
            boost::bind(&connection::handle_read, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred)));
    }
  }

  
}
}
