#ifndef __HTTPS_CLIENT__
#define __HTTPS_CLIENT__

#include "./root_certificates.hpp"

#include <iostream>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

namespace HTTP_CLIENT
{

    class HttpsClient
    {
    public:
        // default constructor
        HttpsClient()
            : ctx(ssl::context::tlsv12_client), resolver(ioc), stream(ioc, ctx)
        {
            init();
        }

        // constructor with parameters
        HttpsClient(const char *host, const char *port, const char *target, int version)
            : _host(host), _port(port), _target(target), _version(version), ctx(ssl::context::tlsv12_client), resolver(ioc), stream(ioc, ctx)
        {
            init();
        }

        // initializing HttpsClient
        // function gets called from within the Constructors
        void init()
        {
            // This holds the root certificate used for verification
            load_root_certificates(ctx);

            // Verify the remote server's certificate
            ctx.set_verify_mode(ssl::verify_peer);

            // Set SNI Hostname (many hosts need this to handshake successfully)
            if (!SSL_set_tlsext_host_name(stream.native_handle(), _host))
            {
                beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
                throw beast::system_error{ec};
            }

            // Look up the domain name
            auto const results = resolver.resolve(_host, _port);

            // Make the connection on the IP address we get from a lookup
            beast::get_lowest_layer(stream).connect(results);

            // Perform the SSL handshake
            stream.handshake(ssl::stream_base::client);

            // Set up an HTTP GET request message
            request.set(http::field::host, _host);
            request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        }

        // closing stream when destructor is called
        ~HttpsClient()
        {
            beast::error_code ec;
            stream.shutdown(ec);
            if (ec == net::error::eof)
            {
                // Not necessarily an error, happens when the stream is already closed
                ec = {};
            }
            if (ec)
            {
                std::cerr << "Error during stream shutdown: " << ec.message() << std::endl;
            }
        }

        // sending http request
        void write()
        {
            http::write(stream, request);
        }

        // receiving http response
        void read()
        {
            http::read(stream, buffer, response);
        }

        // writing response to standard out
        void output()
        {
            std::cout << response << std::endl;
        }

        // host SETTER
        void host(const char *host)
        {
            _host = std::move(host);
        }

        // port SETTER
        void port(const char *port)
        {
            _port = std::move(port);
        }

        // target SETTER
        void target(const char *target)
        {
            _target = std::move(target);
        }

        // version SETTER
        void version(const int version)
        {
            _version = version;
        }

        // member function to change the http method
        void method(const char *method)
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

    private:
        const char *_host;
        const char *_port;
        const char *_target;
        int _version;

        // The io_context is required for all I/O
        net::io_context ioc;

        // The SSL context is required, and holds certificates
        ssl::context ctx;

        // request object
        http::request<http::string_body> request{http::verb::get, _target, _version};

        // container that will hold the response
        http::response<http::dynamic_body> response;

        // These objects perform our I/O
        tcp::resolver resolver;
        beast::ssl_stream<beast::tcp_stream> stream;

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;
    };

};

#endif