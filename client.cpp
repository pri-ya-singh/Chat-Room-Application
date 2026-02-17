#include "message.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <thread>

using boost::asio::ip::tcp;

std::string username;

// ================= READ =================
void async_read(tcp::socket &socket) {

    auto buffer = std::make_shared<boost::asio::streambuf>();

    boost::asio::async_read_until(socket, *buffer, "\n",
        [&socket, buffer](boost::system::error_code ec, std::size_t length) {

            if (!ec) {
                std::istream is(buffer.get());
                std::string received;
                std::getline(is, received);

                std::cout << received << std::endl;

                async_read(socket);
            }
        });
}

// ================= MAIN =================
int main(int argc, char* argv[]) {

    std::cout << "Enter your name: ";
    std::getline(std::cin, username);

    try {

        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);

        boost::asio::connect(socket,
            resolver.resolve("127.0.0.1", argv[1]));

        std::cout << "Connected to chat!\n";

        async_read(socket);

        std::thread t([&]() {

            while (true) {

                std::string msg;
                std::getline(std::cin, msg);

                if(msg == "exit") {
                    socket.close();
                    break;
                }

                std::string fullMsg = username + ": " + msg + "\n";
                std::cout << std::endl;              // move to new line
                std::cout << "You: " << msg << std::endl;
                std::cout << std::flush;

                boost::asio::post(io_context,
                    [&, fullMsg]() {
                        boost::asio::write(socket,
                            boost::asio::buffer(fullMsg));
                    });
            }
        });

        io_context.run();
        t.join();
    }

    catch (std::exception& e) {
        std::cout << e.what();
    }

    return 0;
}
