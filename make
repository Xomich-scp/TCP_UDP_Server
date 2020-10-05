all: server

server: Server_main.o upd_server.o tcp_server.o 
	g++ Server_main.o upd_server.o tcp_server.o -o server -lpthread

Server_main.o: Server_main.cpp
	g++ -c Server_main.cpp

upd_server.o:
	g++ -c upd_server.cpp

tcp_server.o:
	g++ -c tcp_server.cpp



clean:
	rm -rf *.o server
