#include "http.h"
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

bool http::does_url_exist(std::string_view host, std::string_view target)
{
    boost::asio::io_context io_context = { };

    boost::beast::error_code error_code = { };
    boost::asio::ip::tcp::resolver resolver(io_context);

    // TODO: don't use user's dns servers to resolve host
    std::optional<boost::asio::ip::tcp::resolver::results_type> results = resolver.resolve(host, "80", error_code);

    if (error_code.failed() == true || results.has_value() == false)
    {
        return false;
    }

    boost::beast::tcp_stream stream(io_context);

    stream.connect(*results, error_code);

    if (error_code.failed() == true)
    {
        return false;
    }

    boost::beast::http::request<boost::beast::http::empty_body> http_request = { boost::beast::http::verb::head, target, 11 };
    http_request.set(boost::beast::http::field::host, host);
    http_request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    boost::beast::http::write(stream, http_request, error_code);

    if (error_code.failed() == true)
    {
        return false;
    }

    boost::beast::http::response<boost::beast::http::empty_body> response = { };
    boost::beast::flat_buffer buffer = { };

    boost::beast::http::read(stream, buffer, response, error_code);

    if (error_code.failed() == true)
    {
        return false;
    }

    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, error_code);

    if (error_code.failed() == true)
    {
        return false;
    }

    return response.result() != boost::beast::http::status::not_found;
}

