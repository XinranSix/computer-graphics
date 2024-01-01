#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <memory>

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket)
        : socket_(std::move(socket)), request_buffer_(), response_buffer_() {
    }

    void start() {
        // 异步读取请求
        boost::asio::async_read_until(
            socket_, request_buffer_, "\r\n\r\n",
            std::bind(&Session::handle_read, shared_from_this(),
                      std::placeholders::_1, std::placeholders::_2));
    }

private:
    void handle_read(const boost::system::error_code &ec,
                     std::size_t /*length*/) {
        if (!ec) {
            // 读取请求
            std::istream request_stream(&request_buffer_);
            std::string request;
            std::getline(request_stream, request);

            // 构造简单的 HTML 页面
            std::ostream response_stream(&response_buffer_);
            response_stream << "HTTP/1.1 200 OK\r\n";
            response_stream << "Content-Type: text/html\r\n";
            response_stream << "Content-Length: " << 29 << "\r\n";
            response_stream << "\r\n";
            response_stream
                << "<html><body><h1>Hello, World!</h1></body></html>";

            // 异步写入响应
            boost::asio::async_write(
                socket_, response_buffer_,
                std::bind(&Session::handle_write, shared_from_this(),
                          std::placeholders::_1, std::placeholders::_2));
        } else {
            std::cerr << "Error: " << ec.message() << std::endl;
        }
    }

    void handle_write(const boost::system::error_code &ec,
                      std::size_t /*length*/) {
        if (!ec) {
            std::cout << "Response sent successfully!" << std::endl;
            // 清空缓冲区，准备处理下一个请求
            request_buffer_.consume(request_buffer_.size());
            response_buffer_.consume(response_buffer_.size());
            // 异步读取下一个请求
            start();
        } else {
            std::cerr << "Error: " << ec.message() << std::endl;
        }
    }

    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf request_buffer_;
    boost::asio::streambuf response_buffer_;
};

class Server {
public:
    Server(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, boost::asio::ip::tcp::endpoint(
                                    boost::asio::ip::tcp::v4(), port)),
          socket_(io_context) {
        start_accept();
    }

private:
    void start_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket_))->start();
            }
            start_accept(); // 继续等待下一个连接
        });
    }

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
};

int main() {
    boost::asio::io_context io_context;
    Server server(io_context, 8080);
    io_context.run();
    return 0;
}
