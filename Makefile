# Simple makefile for CS 456 A1 for both client.cpp and server.cpp

all: client server

client: client.cpp
	@echo "Compiling client..."
	g++ -std=c++14 -o client client.cpp -Wall

server: server.cpp
	@echo "Compiling server..."
	g++ -std=c++14 -o server server.cpp -Wall

clean:
	rm client server
