#ifndef __CWF_CWF_H__
#define __CWF_CWF_H__

// C++ Web (Devlopment) Framework
//////////////////////////////////////////////////////////////////////////
// 1 nginx + fastcgi devkit => samples
// 2 fcgi host, test => fcgi client
// 3 

#include <boost/asio.hpp>

namespace cwf {

class Request;
class Reply;
class Connection;

class Handler {
public:
    explicit Handler(const std::string &doc_root);
    void Render(const Request &, Reply &);

private:
    const std::string doc_root_;

    /// Templates
};

class Connection 
    : public boost::enable_shared_from_this<Connection>
    , private boost::noncopyable {
public:
    explicit connection(boost::asio::io_service& io_service,
        Handler& handler) : strand_(io_service)
        , socket_(io_service)
        , request_handler_(handler)
        , got_header_(false) , readed_(false) {}

    /// Get the socket associated with the connection.
    boost::asio::ip::tcp::socket& socket();

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

    /// The handler used to process the incoming request.
    Handler& request_handler_;

    std::size_t readed_;

    /// Buffer for incoming data.
    boost::array<char, 8192> buffer_;

    /// The incoming request.
    Request request_;

    /// The parser for the incoming request.
    request_parser request_parser_;

    /// The reply to be sent back to the client.
    Reply reply_;

    bool got_header_;
};

class Request {
public:    
    // header, body, address
};

class Reply {
public:
    // ostringstream?
    // gzip stream
};

class Server {
public:
    explicit Server(const std::string& address, const std::string& port,
        const std::string& doc_root, std::size_t thread_pool_size);

    /// run the io_service loop:
    /// Wait fastcgi connection
    /// Generate Request
    /// url map ===>  Handler
    /// Handler.Render(Request)
    void Run();

    void Stop();

    /// 
    int set_thread_pool_size(std::size_t thread_pool_size);

private:
    /// Handler completion of an asynchronous accept operation.
    void HandleAccept(const boost::system::error_code& e);

    /// The number of threads that will call io_service::run().
    std::size_t thread_pool_size_;

    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    /// The next connection to be accepted.
    connection_ptr new_connection_;

    /// The handler for all incoming requests.
    Handler request_handler_;
};

//////////////////////////////////////////////////////////////////////////
// ??
class Data {};

class TemplateHandle {
};

}

#endif __CWF_CWF_H__
