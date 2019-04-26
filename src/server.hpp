#pragma once

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <reactor/session.hxx>
#include <reactor/listener.hxx>
#include <basic_router.hxx>

#include <thread>
#include "response.hpp"

static boost::asio::io_context ioc;
// static boost::asio::posix::stream_descriptor out{ioc, ::dup(STDOUT_FILENO)};
static boost::asio::signal_set sig_set(ioc, SIGINT, SIGTERM);

using HttpSession = http::reactor::_default::session_type;
using HttpListener = http::reactor::_default::listener_type;


auto make_router() {
    using namespace _0xdead4ead;

    http::basic_router <HttpSession> router{boost::regex::ECMAScript};
    router.get(R"(^/1$)", [](auto request, auto context) {
        //http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "GET 1\n"));
    });
    router.get(R"(^/2$)", [](auto request, auto context) {
        //http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "GET 2\n"));
    });
    const auto &onError = [](auto code, auto from) {
        //http::out::prefix::version::time::pushn<std::ostream>(out, "From:", from, "Info:", code.message());
        if (code == boost::system::errc::address_in_use) {
            ioc.stop();
        }
    };
    const auto &onAccept = [&](auto asioSocket) {
        // http::out::prefix::version::time::pushn<std::ostream>(out, asioSocket.remote_endpoint().address().to_string(), "connected!");
        HttpSession::recv(std::move(asioSocket), router, onError);
    };
    return std::make_pair(onAccept, onError);
}


void server(const std::string &address_, const unsigned short port) {
    auto[onAccept, onError] = make_router();
    auto const address = boost::asio::ip::make_address(address_);

    // http::out::prefix::version::time::pushn<std::ostream>(out, "Start accepting on", address.to_string());
    HttpListener::launch(ioc, {address, port}, onAccept, onError);

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    sig_set.async_wait([](boost::system::error_code const &, int sig) {
        // http::out::prefix::version::time::pushn<std::ostream>(out, "Capture", sig == SIGINT ? "SIGINT." : "SIGTERM.", "Stop!");
        ioc.stop();
    });

    uint32_t pool_size = std::thread::hardware_concurrency() * 2;
    std::vector<std::thread> threads;
    threads.reserve(pool_size > 0 ? pool_size : 4);
    for (uint32_t i = 0; i < pool_size; i++)
        threads.emplace_back(std::bind(static_cast<std::size_t (boost::asio::io_context::*)()>
                                       (&boost::asio::io_context::run), std::ref(ioc)));

    // Block until all the threads exit
    for (auto &t : threads)
        t.join();
}
