#include "chatRoom.hpp"

// ================= ROOM CLASS =================

// Add a participant (client session) to the room
void Room::join(ParticipantPointer participant){
    this->participants.insert(participant);
}
 
// Remove a participant from the room
void Room::leave(ParticipantPointer participant){
    this->participants.erase(participant);
}

// Deliver message from one participant to all others
void Room::deliver(ParticipantPointer participant, Message &message){
    messageQueue.push_back(message);

    // Process all messages in queue
    while (!messageQueue.empty()) {
        Message msg = messageQueue.front();
        messageQueue.pop_front(); 
        
        // Send message to all participants except sender
        for (ParticipantPointer _participant : participants) {
            if (participant != _participant) {
                _participant->write(msg);
            }
        }
    }
}


// ================= SESSION CLASS =================

// Asynchronously read data from client
void Session::async_read() {
    auto self(shared_from_this()); // keep session alive during async call

    boost::asio::async_read_until(clientSocket, buffer, "\n",
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {

            // If no error
            if (!ec) {
                // Convert buffer data into string
                std::string data(boost::asio::buffers_begin(buffer.data()), 
                                 boost::asio::buffers_begin(buffer.data()) + bytes_transferred);

                buffer.consume(bytes_transferred); // remove processed data

                std::cout << "Received: " << data << std::endl;
                
                Message message(data);   // Create Message object
                deliver(message);       // Send to room
                async_read();           // Continue reading next message
            } 
            else {
                // If error, remove session from room
                room.leave(shared_from_this());

                if (ec == boost::asio::error::eof) {
                    std::cout << "Connection closed by peer" << std::endl;
                } else {
                    std::cout << "Read error: " << ec.message() << std::endl;
                }
            }
        }
    );
}


// Asynchronously write data to client socket
void Session::async_write(std::string messageBody, size_t messageLength){

    auto write_handler = [&](boost::system::error_code ec, std::size_t bytes_transferred){

        if(!ec){
            std::cout<<"Data is written to the socket"<<std::endl;
        }else{
            std::cerr << "Write error: " << ec.message() << std::endl;
        }
    };

    boost::asio::async_write(
        clientSocket,
        boost::asio::buffer(messageBody, messageLength),
        write_handler
    );
}


// Start session when client connects
void Session::start(){
    room.join(shared_from_this());  // Add client to room
    async_read();                   // Start listening
}


// Session constructor
Session::Session(tcp::socket s, Room& r)
    : clientSocket(std::move(s)), room(r){};


// Write message to this session
void Session::write(Message &message){

    messageQueue.push_back(message);

    // Process message queue
    while(messageQueue.size() != 0){

        Message message = messageQueue.front();
        messageQueue.pop_front();

        bool header_decode = message.decodeHeader();

        if(header_decode){
            std::string body = message.getBody(); 
            async_write(body, message.getBodyLength());
        }else{
            std::cout<<"Message length exceeds the max length"<<std::endl;
        }
    }
}


// Deliver message to room
void Session::deliver(Message& incomingMessage){
    room.deliver(shared_from_this(), incomingMessage);
}


// ================= ACCEPT CONNECTION FUNCTION =================

using boost::asio::ip::address_v4;

// Accept incoming client connections asynchronously
void accept_connection(boost::asio::io_context &io,
                       char *port,
                       tcp::acceptor &acceptor,
                       Room &room,
                       const tcp::endpoint& endpoint) {

    tcp::socket socket(io);

    acceptor.async_accept(
        [&](boost::system::error_code ec, tcp::socket socket) {

            if(!ec) {
                // Create new session for connected client
                std::shared_ptr<Session> session =
                    std::make_shared<Session>(std::move(socket), room);

                session->start();
            }

            // Keep accepting next connections
            accept_connection(io, port, acceptor, room, endpoint);
        }
    );
}


// ================= MAIN FUNCTION =================

int main(int argc, char *argv[]) {

    try {

        // Check if port number provided
        if(argc < 2) {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        Room room;  // Create chat room

        boost::asio::io_context io_context; // Core I/O engine

        // Create endpoint (IP + Port)
        tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));

        // Create acceptor to listen for clients
        tcp::acceptor acceptor(
            io_context,
            tcp::endpoint(tcp::v4(), std::atoi(argv[1]))
        );

        // Start accepting connections
        accept_connection(io_context, argv[1], acceptor, room, endpoint);

        // Run the IO context (event loop)
        io_context.run();
    }

    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
