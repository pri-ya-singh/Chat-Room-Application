# Compiler
CXX = g++

# Boost path (change if your folder is different)
BOOST_PATH = C:/boost_1_82_0

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra -g -I$(BOOST_PATH)

# Linker flags (Windows needs ws2_32 for networking)
LDFLAGS = -lws2_32 -lboost_system -lboost_thread

# Source files
SERVER_SRC = chatRoom.cpp
CLIENT_SRC = client.cpp

# Object files
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)

# Default target
all: chatApp.exe clientApp.exe

# Server build
chatApp.exe: $(SERVER_OBJ)
	$(CXX) $(SERVER_OBJ) -o chatApp.exe $(LDFLAGS)

# Client build
clientApp.exe: $(CLIENT_OBJ)
	$(CXX) $(CLIENT_OBJ) -o clientApp.exe $(LDFLAGS)

# Compile rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule (Windows version)
clean:
	del /Q *.o *.exe
