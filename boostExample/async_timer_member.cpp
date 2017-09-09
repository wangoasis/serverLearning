// Timer.4
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#define TIME_INTERVAL 1

class printer {
public:
    printer(boost::asio::io_service& io)
    :   t(io, boost::posix_time::seconds(TIME_INTERVAL)),
        count(0) {
		// bind to a pointer to member function
		// pass this as the second parameter to bind()
        t.async_wait(boost::bind(&printer::print, this));
    }

    ~printer() {
        std::cout << "final count: " << count << std::endl;
    }

    void print() {
        if (count < 5) {
            std::cout << "one second passed, count " << count << std::endl;
            ++count;
            // extend expiration time for a interval
            t.expires_at(t.expires_at() + boost::posix_time::seconds(TIME_INTERVAL));
            t.async_wait(boost::bind(&printer::print, this));
        }
    }

private:
    boost::asio::deadline_timer t;
    int count;
};


int main() {

    boost::asio::io_service io;
    printer p(io);
    io.run();
    return 0;
}
