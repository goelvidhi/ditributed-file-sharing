//============================================================================
// Name        : dhtserver3.cpp
// Author      : Vidhi Goel
// Version     :
// Copyright   : Your copyright notice
// Description : Server3 in C++ Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "dhtserver.h"

using namespace std;

# define SERVERNAME "nunki.usc.edu"
# define PORT "23024"
# define BACKLOG 2
# define MAXBUFLEN 100

string key[12], value[12];
int endIndex;
string valueToSend;

// This api converts the hostname to IP address with the help of getaddrinfo() and inet_ntoa conversion
int convertHostToIP(char *ip)
{
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_in *address;
  int rv;
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; 
  hints.ai_socktype = SOCK_STREAM;
   
  if ((rv = getaddrinfo(SERVERNAME, PORT, &hints, &servinfo)) != 0)
    {
      
    }
// loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) 
    {
        address = (struct sockaddr_in *) p->ai_addr;
        strcpy(ip , inet_ntoa(address->sin_addr));
    }
     
    freeaddrinfo(servinfo);
    return 0;
}

// Execution starts from here
int main() {
    char ip[100];
    convertHostToIP(ip);
    cout << "The Server 3 has TCP port number " << PORT << " and IP address " << ip << " ." << endl; 
	readFromFile();
	manageTCPServer3Socket();
	return 0;
}

// This api reads the contents of the file and stores them into array of string
void readFromFile()
{
	char * inputFilePath = "../files/server3.txt";
	string inputKey;
	string inputValue;
	ifstream inputFile;
	inputFile.open(inputFilePath, ios::in);

	if(!inputFile)
	{
		cerr <<"Cant open input file" <<"server.txt" << endl;
		exit(1);
	}

	int i = 0;
	while(!inputFile.eof())
	  {
		inputFile >> inputKey >> inputValue;
		key[i] =inputKey ;
		value[i] = inputValue;
		  i++;

	}
	endIndex = i-1;
	for(int j=0;j<= endIndex;j++)
	  {
	    //     cout<<key[j]<<"\t"<<value[j]<<endl;
	  }
}

// #Beej :: This api gets sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

     return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// This api gets port, IPv4 or IPv6:
in_port_t get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}


// This api takes input key and compares it with stored array to find out the matching value. It returns 0 if success, else returns failure 
int processInputRequest(char *bufKey)
{
  int i;
 

  for (i = 0; i <= endIndex; i++)
    {
      const char* keyChar = key[i].c_str();
      if(strcmp(bufKey,keyChar) != 0)
	continue;
      // cout <<"Match found for the input string with value : " << value[i] << endl;
      valueToSend = "POST " +  value[i];
      return 0;
    }
  
  if(i > endIndex)
    {
      //     cout <<"Match not found at Server2. Please hold on, will try at Server 3.";
      return 1;
    }
}

// This api converts any request/response message into the corresponding key/value by removing "GET/POST" from it
void convertMessageToKeyValue(char *buf, char *buftemp)
{
  char *data = strtok (buf," ");
  if(data)
        data = strtok(NULL, " ");
       
    strcpy(buftemp, data);

}

// This api handles the TCP connection between Server3 and Server2 to receive/send the requests and response for key/value 
void manageTCPServer3Socket()
{
  int status;
  char buf[MAXBUFLEN], bufCopy[MAXBUFLEN], bufKey[MAXBUFLEN];
  struct addrinfo hints, *serverInfo, *p;
  int sockfd, child_fd;
  struct sockaddr_storage remote_addr; // connector's address information
  socklen_t sin_size;
  int yes = 1;
  char remoteIP[INET6_ADDRSTRLEN];
  int numbytes;
  
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

	// #Beej :: getaddrinfo usage
  if((status = getaddrinfo(SERVERNAME, PORT, &hints, &serverInfo))!= 0){
    cerr <<"Server3 getaddrinfo error:: " << gai_strerror(status)<< endl;
    exit(1);
  }

  // #Beej :: socket creation usage
  for(p = serverInfo; p != NULL; p = p->ai_next)
    {
      if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))== -1){
	cerr <<"Server3 :: Socket Failure" << endl;
	continue;
      }
    
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
	cerr<<"Server 3 :: setsockopt failed" << endl;
            exit(1);
        }

		// #Beej :: bind usage
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            cout<<"Server3 :: bind failed" << endl;
            continue;
        }

        break;	
    }

  if (p == NULL)  {
    cerr << "Server3 :: failed to bind"<< endl;
        return ;
    }

  freeaddrinfo(serverInfo);
  
  // #Beej :: listen usage
  if (listen(sockfd, BACKLOG) == -1) {
    cerr << "Server3 listen :: backlog has reached an end" <<endl ;
        exit(1);
    }

  //  cout<<"Server3 :: waiting for connections...\n";

    while(1) {  // #Beej :: accept() loop
        sin_size = sizeof remote_addr;
        child_fd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
        if (child_fd == -1) {
	  cerr<<"Server3 accept :: failed to create child socket" << endl;
            continue;
        }
	inet_ntop(remote_addr.ss_family,get_in_addr((struct sockaddr *)&remote_addr),remoteIP, sizeof remoteIP);
        int remotePort = ntohs(get_in_port((struct sockaddr *)&remote_addr));
      
	//  printf("Server3 :: got connection from %s\n", s);
	
	    if ((numbytes = recv(child_fd, buf, MAXBUFLEN-1, 0)) == -1) {
	      perror("recv");
	      exit(1);
	    }

	    buf[numbytes] = '\0';
	    strcpy(bufCopy, buf);
	    convertMessageToKeyValue(bufCopy, bufKey);
	    cout<<"The Server 3 has received a request with key " <<bufKey << " from Server 2 with port number " <<remotePort << " and IP address " <<remoteIP << " . \n";
	    int l_status = processInputRequest(bufKey);
	    
    if(l_status == 0)
      {
	//	sendUDPResponseToClient(sockfd,(struct sockaddr *)&remote_addr);
       
	cout << "The Server 3 sends the reply "<< valueToSend <<" to the Server 2 with port number "<< remotePort << " and IP address " << remoteIP << ".\n";
	const char * value = valueToSend.c_str();
  
	if ((numbytes = send(child_fd, value, strlen(value), 0)) == -1) {
	  perror(" Server3 :: sendto");
	  exit(1);
	}
       
      }
        
        close(child_fd);  // parent doesn't need this
    }// end of while

}

