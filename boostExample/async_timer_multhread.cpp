// Timer.5
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define TIME_INTERVAL 1

class printer {
public:
    printer(boost::asio::io_service &io) 
    : strand(io), 
      t1(io, boost::posix_time::seconds(TIME_INTERVAL)), 
      t2(io, boost::posix_time::seconds(TIME_INTERVAL)) {
        /*
            The strand::wrap() function returns a new handler that 
            automatically dispatches its contained handler through 
            the boost::asio::strand object. By wrapping the handlers 
            using the same boost::asio::strand, we are ensuring that 
            they cannot execute concurrently.
        */
        t1.async_wait(strand.wrap(boost::bind(&printer::print1, this)));
        t2.async_wait(strand.wrap(boost::bind(&printer::print2, this)));
    }

    ~printer() {
        std::cout << boost::this_thread::get_id() << " final count: " << count << std::endl;
    }

    void print1() {
        if (count < 10) {
            // for convinience, cout the thread id
            std::cout << boost::this_thread::get_id() << " timer 1: " << count << std::endl;
            ++count;
            // extend expiration time for a interval
            t1.expires_at(t1.expires_at() + boost::posix_time::seconds(TIME_INTERVAL));
            t1.async_wait(boost::bind(&printer::print1, this));
        }
    }

    void print2() {
        if (count < 10) {
            std::cout << boost::this_thread::get_id() << " timer 2: " << count << std::endl;
            ++count;
            // extend expiration time for a interval
            t2.expires_at(t2.expires_at() + boost::posix_time::seconds(TIME_INTERVAL));
            t2.async_wait(boost::bind(&printer::print2, this));
        }
    }

private:
    boost::asio::io_service::strand strand;
    boost::asio::deadline_timer t1;
    boost::asio::deadline_timer t2;
    int count; 
};

int main() {
    boost::asio::io_service io;
    printer p(io);
    boost::thread new_thread(boost::bind(&boost::asio::io_service::run, &io));
    io.run();
    new_thread.join();

    return 0;
}