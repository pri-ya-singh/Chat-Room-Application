#include "message.hpp"
#include <iostream>
// #include <thread.hpp>
#include <boost/asio.hpp>
#include <thread>

using boost::asio::ip::tcp;

// ================= ASYNC READ FUNCTION =================
void async_read(tcp::socket &socket) {

    auto buffer = std::make_shared<boost::asio::streambuf>();

    boost::asio::async_read_until(socket, *buffer, "\n",
        [&socket, buffer](boost::system::error_code ec, std::size_t length) {

            if (!ec) {
                std::istream is(buffer.get());
                std::string received;
                std::getline(is, received);

                std::cout << "Server: " << received << std::endl;

                async_read(socket);  // Continue reading
            } 
            else {
                std::cout << "Disconnected from server: "
                          << ec.message() << std::endl;
            }
        }
    );
}

// ================= MAIN =================
int main(int argc, char* argv[]) {

    try {

        if(argc < 2){
            std::cerr << "Usage: client <port>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);

        // Connect to server
        boost::asio::connect(socket,
            resolver.resolve("127.0.0.1", argv[1]));

        std::cout << "Connected to server!" << std::endl;

        // Start async reading
        async_read(socket);

        // Thread for sending messages
        std::thread t([&]() {

            while (true) {

                std::string data;
                std::cout << "Enter message: ";
                std::getline(std::cin, data);

                if(data == "exit") {
                    socket.close();
                    break;
                }

                data += "\n";

                boost::asio::post(io_context,
                    [&, data]() {
                        boost::asio::write(socket,
                            boost::asio::buffer(data));
                    });
            }
        });

        io_context.run();
        t.join();
    }

    catch (std::exception& e) {
        std::cerr << "Exception: "
                  << e.what() << std::endl;
    }

    return 0;
}
