all: servers clients

servers: server1 server2 server3

# make server1 executable
server1: dhtserver1.cpp
	g++ -o server1 dhtserver1.cpp -lsocket -lnsl -lresolv

#make server2 executable
server2: dhtserver2.cpp
	g++ -o server2 dhtserver2.cpp -lsocket -lnsl -lresolv

#make server3 executable
server3: dhtserver3.cpp
	g++ -o server3 dhtserver3.cpp -lsocket -lnsl -lresolv

clients: client1 client2
# make client1 executable
client1: client1.cpp
	g++ -o client1 client1.cpp -lsocket -lnsl -lresolv

# make client2 executable
client2: client2.cpp
	g++ -o client2 client2.cpp -lsocket -lnsl -lresolv