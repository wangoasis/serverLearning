#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

    boost::asio::io_service io_service;

    // The ip::basic_resolver class template provides the ability to resolve a query to a list of endpoints.
    tcp::resolver resolver(io_service);

    // query(host, service_name)
    tcp::resolver::query query(argv[1], "daytime");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    tcp::socket socket(io_service);
    // establish a socket connection by trying each endpoint in a sequence
    boost::asio::connect(socket, endpoint_iterator);

    for (;;) {
      boost::array<char, 128> buf;
      boost::system::error_code error;

      size_t len = socket.read_some(boost::asio::buffer(buf), error);

      if (error == boost::asio::error::eof)
        break; // read until end of file
      else if (error)
        throw boost::system::system_error(error);

      std::cout.write(buf.data(), len);
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}