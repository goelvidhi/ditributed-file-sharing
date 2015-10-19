Author
Vidhi Goel
USC ID: 5386 3530 24

(Last three digits of my USC ID = 024, so the Port numbers are 21024, 22024, 23024 for server1, server2 and server3 respectively)
----------------------------------------------------------------------------------------------------------------------------------------------
Introduction
The project SocketProgramming has been developed in C++ to simulate the client server interaction in both TCP and UDP manner. The clients post their requests to a UDP server. The server either replies to client request or take help of other TCP servers in a recursive manner. The servers update their cache to avoid future requests of same items.
----------------------------------------------------------------------------------------------------------------------------------------------
Project Description
The project contains two folders namely "src" and "files". The folder "src" folder contains all the header files, source code, Makefile and Readme file. The folder "files" should contain text files for clients and servers. The code reads text files from "files" folder placed outside "src". Please make sure to add files at the right place. The sample files as provided in the project specification are already present in "files" folder.
 
The source code with below files is present "src":
1. client.h - header file that contains declaration of methods used by client1.cpp and client2.cpp

2. client1.cpp - code file that defines the methods implemented by client1. It parses client1.txt to store the search string and their corresponding key mappings. It takes input from the user for the search string and send a UDP request to server for its corresponding value. I have used getaddrinfo() to get the IP address of the client with argument = "localhost".

3. client2.cpp - code file that performs same functions as client1.cpp except that it reads from client2.txt.  I have used getaddrinfo() to get the IP address of the client with argument = "localhost".

4. dhtserver.h - header file that contains declaration of methods used by dhtserver1.cpp, dhtserver2.cpp and dhtserver3.cpp

5. dhtserver1.cpp - (UDP server) code file that parses server1.txt to store the key-value mappings in local storage. It reads the key sent by the client via UDP and performs a search in its local storage to find the corresponding value. If value is found, it returns the same to the client and closes the socket. Else, it creates a TCP connection to Server2 and requests for the value by sending the key.

6. dhtserver2.cpp - (TCP server) code file that parses server2.txt to store the key-value mappings in local storage. It creates a TCP parent socket and listens for incoming connection. It reads the key sent by Server1 over TCP connection and tries to find its corresponding value in its storage. If found, it returns the same to Server1. Else, it creates a TCP client socket to connect to Server3 which is another TCP server. It sends the key over this connection after connect is successful.

7. dhtserver3.cpp - (TCP server) code file that parses server3.txt to store the key-value mappings in local storage. It create a TCP parent socket and listens to incoming connections. It reads the key sent by Server2 over TCP connection and searches for its corresponding value in its storage. As this is the last Server, it will definitely find a match for the key (as mentioned in specification). It will return the matched value to server2.

8. Makefile - It contains the commands to compile the source code into executables.


----------------------------------------------------------------------------------------------------------------------------------------------
Steps to follow:
1. The text files were orginally placed inside "files" folder to segregate data from code. To add text files please add them to "files" folder because the code reads files from that folder.
2. To compile all the source files and convert to executable, run the following command inside "src" folder that contains Makefile:

 >> make all

This command will create executable for 3 servers and 2 clients. 

3. To execute these, use the following commands in order on different terminals:
./server1
./server2
./server3

./client1
./client2

4. Please keep the servers running on their individual terminal throughout the evaluation.
5. You may terminate the servers by CTRL + C command or any other applicable command.
------------------------------------------------------------------------------------------------------------------------------------------------
4. Message Exchange format
I have used the same format as specified in the project specifications with "GET keyxx" for request message and "POST valuexx" for response message.

------------------------------------------------------------------------------------------------------------------------------------------------
5. Failure Cases
In the below conditions, the code might fail. This conditions will never occur as already mentioned in the project specifications

1. User enters a search string that does not match any of the values present in client1.txt (client2.txt).
2. The text file client1.txt and client2.txt contains more than 12 records each.
3. The text file server1.txt, server2.txt and server3.txt contains more than 4 records each.
4. During execution at my end, I never encountered an error like "port already in use" or "address already in use", but in case, if you encouter the same, please terminate any unnecessary process, if any.

-------------------------------------------------------------------------------------------------------------------------------------------------
6. Reused Code
I used code snippets from Beej. It has been marked by "#Beej" tag in code. They are as follows:
1. void *get_in_addr(struct sockaddr *sa)
2. getaddrinfo usage
3. socket creation
4. socket bind procedure
5. sendto and recvfrom usage
6. connect, accept and listen usage for TCP
7. send/recv for TCP connection