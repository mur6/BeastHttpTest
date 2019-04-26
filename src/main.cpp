//#include "server.hpp"
//
//int main() {
//    //auto const port = static_cast<unsigned short>(3388);
//    server("0.0.0.0", 3388);
//    return 0;
//}

#include <boost/asio.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

boost::asio::io_service io_service;

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>



auto main() -> int {
    boost::asio::io_context ctx{};

    boost::asio::posix::stream_descriptor stream_in{ctx, ::dup(STDIN_FILENO)};
    boost::asio::posix::stream_descriptor stream_out{ctx, ::dup(STDOUT_FILENO)};

    boost::asio::streambuf buffer{};
    boost::system::error_code error{};

    while (boost::asio::read(stream_in, buffer, boost::asio::transfer_at_least(1), error)) {
        boost::asio::write(stream_out, buffer);
    }

    if (error != boost::asio::error::eof) {
        std::cerr << error.message() << std::endl;
        return 1;
    }
}
