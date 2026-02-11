# 💬 chatRoomCpp

## 📌 Project Overview

chatRoomCpp is a multi-client chat room application developed in **C++** using **Boost.Asio** for asynchronous network communication.  
The application enables multiple clients to connect to a server and exchange messages in real time over TCP/IP.

It demonstrates practical implementation of:

- Asynchronous socket programming  
- Event-driven architecture  
- Multi-threading  
- Session and resource management  

---

## 🚀 Key Features

- **Asynchronous Communication**  
  Utilizes Boost.Asio for non-blocking I/O operations, allowing concurrent client handling without blocking the server.

- **Multi-client Support**  
  Multiple clients can connect simultaneously and communicate in a shared chat room.

- **Threading Support**  
  Uses multi-threading to handle user input and network operations concurrently.

- **Room Management**  
  Implements a room abstraction to manage connected participants efficiently.

- **Message Handling System**  
  Structured message class for encoding, decoding, and validating message headers and body.

---

## 🏗️ Architecture

The project is structured into the following core components:

### 1️⃣ Session
Represents an individual client connection.

Responsibilities:
- Manages client socket
- Performs asynchronous read and write operations
- Delivers incoming messages to the room
- Handles client disconnection safely

---

### 2️⃣ Room
Represents the shared chat environment.

Responsibilities:
- Maintains a set of connected participants
- Adds and removes clients
- Broadcasts messages to all active participants
- Ensures synchronized message delivery

---

### 3️⃣ Message
Encapsulates the data exchanged between clients.

Responsibilities:
- Encodes message header
- Decodes message header
- Validates message length
- Provides structured access to message body

---

## ⚙️ Critical Functionalities

### 🔹 Asynchronous I/O

The application uses:

- `boost::asio::async_read`
- `boost::asio::async_write`

These enable:
- Non-blocking communication
- Event-driven message handling
- Efficient resource utilization

---

### 🔹 Multi-threading

The client application uses threading to:
- Handle user input independently
- Send messages while receiving incoming messages simultaneously

This ensures smooth real-time communication.

---

### 🔹 Session Management

Each connected client is represented by a `Session` object.

The session:
- Joins the room
- Listens for incoming messages
- Delivers messages to other participants
- Cleans up resources on disconnect

---

### 🔹 Room Management

The `Room` class:
- Maintains a container of active participants
- Broadcasts incoming messages
- Handles safe addition and removal of clients

---

## 🛠️ Technologies Used

- **C++ (C++17 / C++20)**
- **Boost.Asio**
- **TCP/IP Socket Programming**
- **Multi-threading**
- **Object-Oriented Design**

---

## 🖥️ How to Build

### Compile (MinGW example)

```bash
g++ chatRoom.cpp -std=gnu++17 -I"path_to_boost" -lboost_system -pthread -o chatApp
