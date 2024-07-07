#ifndef __HTTP_CLIENT__
#define __HTTP_CLIENT__

#include <iostream>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace HTTP_CLIENT
{

    class HttpClient
    {
    public:
        HttpClient()
            : _host("example.com"), _port("443"), _target("/"), _version(11), resolver(ioContext), stream(ioContext)
        {
            init();
        }

        HttpClient(const char *host, const char *port, const char *target, int version)
            : _host(std::move(host)), _port(std::move(port)), _target(std::move(target)), _version(version), resolver(ioContext), stream(ioContext)
        {
            init();
        }

        ~HttpClient()
        {
            // close the socket
            beast::error_code errorCode;
            stream.socket().shutdown(tcp::socket::shutdown_both, errorCode);

            if (errorCode && errorCode != beast::errc::not_connected)
                throw beast::system_error(errorCode);
        }

        void init()
        {
            // perform domain name lookup
            auto const results = resolver.resolve(_host, _port);

            // connect on IP adress we got through the lookup
            stream.connect(results);

            request.set(http::field::host, _host);
            request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        }

        void host(const char *&host)
        {
            _host = std::move(host);
        }

        void port(const char *&port)
        {
            _port = std::move(port);
        }

        void target(const char *&target)
        {
            _target = std::move(target);
        }

        void version(const int version)
        {
            _version = version;
        }

        void method(const char *&method)
        {

            if (method == "get")
            {
                request.method(http::verb::get);
            }

            if (method == "post")
            {
                request.method(http::verb::post);
            }
        }

        void write()
        {
            http::write(stream, request);
        }

        void read()
        {
            http::read(stream, buffer, response);
        }

        void output()
        {
            std::cout << response << std::endl;
        }

    private:
        const char *_host;
        const char *_port;
        const char *_target;
        int _version;

        // required for all I/O
        net::io_context ioContext;
        // objects that perform I/O
        tcp::resolver resolver;
        beast::tcp_stream stream;
        // object that holds request message
        http::request<http::string_body> request{http::verb::get, _target, _version};
        // buffer for reading
        beast::flat_buffer buffer;
        // container that holds the response
        http::response<http::dynamic_body> response;
    };
    
}
#endif