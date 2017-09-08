#include <iostream>
using namespace std;
#include <boost/asio.hpp>
//using namespace boost::asio;

int main()
{
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket udp_socket(io_service);
    boost::asio::ip::udp::endpoint local_add(boost::asio::ip::address::from_string("127.0.0.1"), 4000);

    udp_socket.open(local_add.protocol());

    char receive_str[1024] = { 0 };
    while (1)
    {
        boost::asio::ip::udp::endpoint  sendpoint;
        string s;
        cin >> s;
        udp_socket.send_to(boost::asio::buffer(s.c_str(),s.size()), local_add);
        udp_socket.receive_from(boost::asio::buffer(receive_str, 1024), local_add);
        cout << "receive " << receive_str << endl;
        memset(receive_str, 0, 1024);
    }
    return 0;
}
