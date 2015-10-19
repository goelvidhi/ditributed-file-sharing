//============================================================================
// Name        : client1.cpp
// Author      : Vidhi Goel
// Version     :
// Copyright   : Your copyright notice
// Description : Client1 in C++ Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fstream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "client.h"

using namespace std;

# define SERVERPORT "21024" 
# define SERVERNAME "nunki.usc.edu"
# define MAXBUFLEN 100
# define OWNADDRESS "localhost"

string key[13], value[13];
string keyToSend;

// This api converts the hostname to IP address with the help of getaddrinfo() and inet_ntoa conversion
int convertHostToIP(char *ip)
{
  struct addrinfo hints, *sinfo, *p;
  struct sockaddr_in *address;
  int rv;
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; 
  hints.ai_socktype = SOCK_STREAM;
   
  if ((rv = getaddrinfo(OWNADDRESS, NULL, &hints, &sinfo)) != 0)
    {
      
    }
// loop through all the results and connect to the first we can
    for(p = sinfo; p != NULL; p = p->ai_next) 
    {
        address = (struct sockaddr_in *) p->ai_addr;
        strcpy(ip , inet_ntoa(address->sin_addr));
    }
     
    freeaddrinfo(sinfo);
    return 0;
}

// Execution starts from here
int main() {
  
	readFromFile();
	int i_status = processUserInput();
	
	if(i_status == 0)
	  {
	    int s_status = manageUDPClient1Socket();
	    if(s_status == 0)
	      {
		
	      }
	  }
	
	return 0;
}

// This api reads the contents of the file and stores them into array of string
void readFromFile()
{
	char * inputFilePath = "../files/client1.txt";
	string inputKey;
	string inputValue;
	ifstream inputFile;
	inputFile.open(inputFilePath, ios::in);

	if(!inputFile)
	{
		cerr <<"Cant open input file " <<"client1.txt" << endl;
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

	for(int j=0;j<12;j++)
	  {
	    //	    cout<<key[j]<<"\t"<<value[j]<<endl;
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

// This api takes user input and compares it with stored array to find out the matching key.
int processUserInput()
{
  string inputString;
  cout<<"Please Enter Your Search(USC, UCLA etc.): " << endl;
  cin>> inputString;
  int i;
  for (i = 0; i < 12; i++)
    {
      if(key[i].compare(inputString) != 0)
	continue;
      
      cout << "The Client 1 has received a request with search word " << inputString<< ", which maps to key " << value[i]<< ".\n";
      keyToSend = "GET " + value[i];
      break;
      
    }
  if (i == 12) return -1;
  else return 0;

}
// This api is for UDP communication with Server1. It sends "GET keyxx" as the request and receives "POST valuexx" as the response from Server1
int manageUDPClient1Socket()
{
    int sockfd;
    struct addrinfo hints, *serverinfo, *p;
    int rv;
    int numbytes;
    char buf[MAXBUFLEN];
    char selfIP[INET6_ADDRSTRLEN], remoteIP[INET6_ADDRSTRLEN];

	convertHostToIP(selfIP);
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

	// #Beej :: getaddrinfo
    if ((rv = getaddrinfo(SERVERNAME, SERVERPORT, &hints, &serverinfo)) != 0) {
      cerr <<  "getaddrinfo: " <<  gai_strerror(rv)<< endl;
        return 1;
    }

    // #Beej :: loop through all the results and make a socket
    for(p = serverinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("Client1 :: socket \n");
            continue;
        }
		
        break;
    }

	
    if (p == NULL) {
      cerr<<  "Client1 :: failed to bind socket\n";
        return 2;
    }
    
	
    const char * key =  keyToSend.c_str();
    
	// #Beej :: sendto
if ((numbytes = sendto(sockfd, key, strlen(key), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror(" Client1 :: sendto");
        exit(1);
    }
    inet_ntop(p->ai_family,get_in_addr((struct sockaddr *)p->ai_addr),remoteIP, sizeof remoteIP);
    int remotePort = ntohs(get_in_port((struct sockaddr *)p->ai_addr));

	//below code is to find own port
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
      perror("getsockname");
    
//      inet_ntop(sin.sin_family, &sin.sin_addr, selfIP, sizeof selfIP);   


 cout << "The Client 1 sends the request " << key << " to the Server 1 with port number " <<remotePort << " and IP address " <<remoteIP << " . \n";
 cout<<"The Client1's port number is " << ntohs(sin.sin_port) <<" and the IP address is " << selfIP<< ".\n";
	// #Beej :: recvfrom
  if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, p->ai_addr, &p->ai_addrlen)) == -1) {
        perror("recvfrom");
        exit(1);
    }
  buf[numbytes] = '\0';
  char bufCopy[MAXBUFLEN];
  strcpy(bufCopy, buf);
  const char* data = strtok (bufCopy," ");
   if(data)
     {
       data = strtok(NULL, " ");
    
     }

   cout << "The Client 1 received the value " << data<< " from the Server 1 with port number " << remotePort<< "  and IP address " <<remoteIP << " . \n";
   cout << "The Client1's port number is " << ntohs(sin.sin_port)<< " and IP address is " << selfIP<< ".\n";
   freeaddrinfo(serverinfo);

    
    close(sockfd);
    
}
