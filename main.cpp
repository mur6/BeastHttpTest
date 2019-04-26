#include <reactor/listener.hxx>
#include <reactor/session.hxx>

#include <basic_router.hxx>
#include <out.hxx>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <thread>
#include "response.cpp"

static boost::asio::io_context ioc;
static boost::asio::posix::stream_descriptor out{ioc, ::dup(STDOUT_FILENO)};
static boost::asio::signal_set sig_set(ioc, SIGINT, SIGTERM);

int main()
{
//    root@x0x0:~# (echo -en "GET /1 HTTP/1.1\nHost: localhost\n\nGET /2 HTTP/1.1\nHost: localhost\n\n"; sleep 0.1) | telnet localhost 80
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/1'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/2'
//    root@x0x0:~# curl localhost --request 'GET' --request-target '/3'

    using namespace _0xdead4ead;

    using HttpSession = http::reactor::_default::session_type;
    using HttpListener = http::reactor::_default::listener_type;

    http::basic_router<HttpSession> router{boost::regex::ECMAScript};

    router.get(R"(^/1$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "GET 1\n"));
    });

    router.get(R"(^/2$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "GET 2\n"));
    });

    router.get(R"(^/3$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "GET 3\n"));
    });

    router.all(R"(^.*$)", [](auto request, auto context) {
        http::out::pushn<std::ostream>(out, request);
        context.send(make_response(request, "ALL\n"));
    });

    const auto& onError = [](auto code, auto from) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "From:", from, "Info:", code.message());

        if (code == boost::system::errc::address_in_use)
            ioc.stop();
    };

    const auto& onAccept = [&](auto asioSocket) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, asioSocket.remote_endpoint().address().to_string(), "connected!");

        HttpSession::recv(std::move(asioSocket), router, onError);
    };

    auto const address = boost::asio::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(3388);

    // Start accepting
    http::out::prefix::version::time::pushn<std::ostream>(
                out, "Start accepting on", address.to_string());
    HttpListener::launch(ioc, {address, port}, onAccept, onError);

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    sig_set.async_wait([](boost::system::error_code const&, int sig) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "Capture", sig == SIGINT ? "SIGINT." : "SIGTERM.", "Stop!");
        ioc.stop();
    });

    uint32_t pool_size = std::thread::hardware_concurrency() * 2;

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> threads;
    threads.reserve(pool_size > 0 ? pool_size : 4);
    for(uint32_t i = 0; i < pool_size; i++)
        threads.emplace_back(std::bind(static_cast<std::size_t (boost::asio::io_context::*)()>
                                       (&boost::asio::io_context::run), std::ref(ioc)));

    // Block until all the threads exit
    for(auto& t : threads)
        t.join();

    return 0;
}
