#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define TIME_INTERVAL 1

void print(const boost::system::error_code& /*e*/, 
    boost::asio::deadline_timer *t, int *count) {
    if (*count < 5) {
        std::cout << "one second passed, count " << *count << std::endl;
        ++(*count);
        // extend expiration time for a interval
        t->expires_at(t->expires_at() + boost::posix_time::seconds(TIME_INTERVAL));
        t->async_wait(boost::bind(print, boost::asio::placeholders::error, t, count));
    }
}

int main() {

    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::seconds(TIME_INTERVAL));

    int count = 0;
    /* 
        The deadline_timer::async_wait() function expects a handler function 
        (or function object) with the signature void(const boost::system::error_code&). 
        Binding the additional parameters converts your print function into a function 
        object that matches the signature correctly.
    */
    t.async_wait(boost::bind(print, boost::asio::placeholders::error, &t, &count));
    std::cout << "start counting" << std::endl;
    io.run();
    std::cout << "final count: " << count << std::endl;
    return 0;
}