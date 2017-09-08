#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void print(const boost::system::error_code& /*e*/) {
    std::cout << "Hello, World from print function" << std::endl;
}

int main() {

    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::seconds(3));

    t.async_wait(&print);
    std::cout << "Hello, World from main function 1" << std::endl;
    io.run();
    std::cout << "Hello, World from main function 2" << std::endl;

    return 0;
}
