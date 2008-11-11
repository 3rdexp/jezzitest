#ifndef __CWF_CWF_H__
#define __CWF_CWF_H__

// C++ Web (Devlopment) Framework
//////////////////////////////////////////////////////////////////////////
// 1 nginx + fastcgi devkit => samples
// 2 fcgi host, test => fcgi client
// 3 

#include <string>
#include <map>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

#include "cwf/fcgi_spec.hpp"

namespace cwf {

class Request;
class Reply;
class Connection;

class Request {
public:
  int request_id() const { return record_.header_.request_id(); }
  void PutParam(const std::string &key, const std::string &val) {
    params_.insert(std::make_pair(key, val));
  }
  
private:
  fcgi::BeginRequestRecord record_;
  typedef std::multimap<std::string, std::string> ParamMap;
  ParamMap params_;
};

class Reply {
public:
  Reply() : header_(fcgi::STDOUT, 0, 0) {}
  Reply(fcgi::request_t type, int request_id, int length) 
    : header_(type, request_id, length) {}

  std::vector<boost::asio::const_buffer> to_buffers() const;
  
private:
  fcgi::Header header_;
};

class Parser {
public:
  Parser() {}

  template <typename InputIterator>
  boost::tuple<boost::tribool, InputIterator> Parse(
          InputIterator begin, std::size_t length, Request & request) {
      boost::tribool result = boost::indeterminate;
      result = Process(reinterpret_cast<const char *>(begin), length, request);
      
      return boost::make_tuple(result, begin);
  }

private:
  bool Process(const char *stream, std::size_t length, Request &request);
};

class Handle {
public:
  Handle(const std::string &doc_root) : doc_root_(doc_root) {}
  bool Render(const Request &req, Reply &reply);

private:
  std::string doc_root_;
};

class Connection 
    : public boost::enable_shared_from_this<Connection>
    , private boost::noncopyable {
public:
    explicit Connection(boost::asio::io_service& io_service,
        Handle& handler) : strand_(io_service)
        , socket_(io_service)
        , request_handler_(handler)
        , got_header_(false) , readed_(false) {}

    /// Get the socket associated with the connection.
    boost::asio::ip::tcp::socket& socket() {
      return socket_;
    }

    /// Start the first asynchronous operation for the connection.
    void Start();
private:
    /// Handle completion of a read operation.
    void HandleRead(const boost::system::error_code& e,
        std::size_t bytes_transferred);

    /// Handle completion of a write operation.
    void HandleWrite(const boost::system::error_code& e);

    /// Strand to ensure the connection's handlers are not called concurrently.
    boost::asio::io_service::strand strand_;

    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    Parser parser_;

    /// The handler used to process the incoming request.
    Handle& request_handler_;

    std::size_t readed_;

    /// Buffer for incoming data.
    boost::array<char, 8192> buffer_;

    /// The incoming request.
    Request request_;

    /// The parser for the incoming request.
    // request_parser request_parser_;

    /// The reply to be sent back to the client.
    Reply reply_;

    bool got_header_;
};

class Server {
public:
    explicit Server(const std::string& address, const std::string& port,
        const std::string& doc_root, std::size_t thread_pool_size) 
        : thread_pool_size_(thread_pool_size)
        , address_(address), port_(port)
        , acceptor_(io_service_)
        , request_handler_(doc_root)  {}

    bool Init();

    /// run the io_service loop:
    /// Wait fastcgi connection
    /// Generate Request
    /// url map ===>  Handle
    /// Handle.Render(Request)
    void Run();

    void Stop();

    /// 
    int set_thread_pool_size(std::size_t thread_pool_size);

private:
    /// Handle completion of an asynchronous accept operation.
    void HandleAccept(const boost::system::error_code& e);

    /// The number of threads that will call io_service::run().
    std::size_t thread_pool_size_;

    std::string address_, port_;

    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    /// The next connection to be accepted.
    // connection_ptr new_connection_;
    boost::shared_ptr<Connection> new_connection_;

    /// The handler for all incoming requests.
    Handle request_handler_;    
};

//////////////////////////////////////////////////////////////////////////
// ??
class Data {};

class TemplateHandle {
};

}

#endif __CWF_CWF_H__
