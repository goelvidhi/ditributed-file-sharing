//============================================================================
// Name        : dhtserver1.cpp
// Author      : Vidhi Goel
// Version     :
// Copyright   : Your copyright notice
// Description : Server1 in C++ Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include "dhtserver.h"

using namespace std;

# define UDPPORT "21024"
# define MAXBUFLEN 100
# define TCPDESTINATIONPORT "22024"
# define SERVERNAME "nunki.usc.edu"

string key[12], value[12];
string valueToSend;
int endIndex;


// This api converts the hostname to IP address with the help of getaddrinfo() and inet_ntoa conversion
int convertHostToIP(char *ip)
{
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_in *address;
  int rv;
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; 
  hints.ai_socktype = SOCK_STREAM;
   
  if ((rv = getaddrinfo(SERVERNAME, UDPPORT, &hints, &servinfo)) != 0)
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
  cout << "The Server 1 has UDP port number " << UDPPORT << " and IP address " << ip << "." << endl; 
	readFromFile();
	manageUDPServer1Socket();
	return 0;
}

// This api reads the contents of the file and stores them into array of string
void readFromFile()
{
	char * inputFilePath = "../files/server1.txt";
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
	//    cout<<key[j]<<"\t"<<value[j]<<endl;
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
  
  for (i = 0; i<= endIndex; i++)
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
      // cout <<"Match not found at Server1. Please hold on, will try at Server 2.";
      return 1;
    }
}

// never used 
void sendUDPResponseToClient(int sockfd, struct sockaddr *remote_addr)
{
  int numbytes;
  socklen_t addr_len;
  cout <<"Server1 :: Returning the value to the client ";
  const char * value = valueToSend.c_str();
  addr_len = sizeof remote_addr;
  if ((numbytes = sendto(sockfd, value, strlen(value), 0, (struct sockaddr *)&remote_addr, addr_len)) == -1) {
      perror(" Server1 :: sendto");
     exit(1);
    }
  cout << "Server 1 :: sent " << numbytes << " bytes to client";

}

// This api updates the cache of Server1 (string array) whenever it receives a new key-value mapping from Server2
void updateLocalDataStructure(char *buf)
{
  char tempValue[MAXBUFLEN], tempKey[MAXBUFLEN];
  strcpy(tempKey, buf);
  strcpy(tempValue, valueToSend.c_str());
  
  const char* keyLocal = strtok (tempKey," ");
    if(keyLocal)
        keyLocal = strtok(NULL, " ");
   
    const char* valueLocal = strtok(tempValue, " ");
    if(valueLocal)
      valueLocal = strtok(NULL, " ");
 
	string k(keyLocal);
	string v(valueLocal);
	++endIndex;
	key[endIndex] = k;
	value[endIndex] = v;
 
}

// This api converts the any request/response message into the corresponding key/value by removing "GET/POST" from it
void convertMessageToKeyValue(char *buf, char *buftemp)
{
  char *data = strtok (buf," ");
  if(data)
        data = strtok(NULL, " ");
       
    strcpy(buftemp, data);

}


// This api handles the UDP connection between Server1 and clients to receive the message "GET keyxx" and send the response "POST valuexx"
void  manageUDPServer1Socket()
{
  int sockfd;
  int clientID = 1;
  struct addrinfo hints, *serverinfo, *p;
  int status;
  int numbytes;
  struct sockaddr_storage remote_addr;
  char buf[MAXBUFLEN], bufCopy[MAXBUFLEN], bufKey[MAXBUFLEN];
  socklen_t addr_len;
  char remoteIP[INET6_ADDRSTRLEN], selfIP[INET6_ADDRSTRLEN];;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    
	// #Beej :: getaddrinfo usage
    if ((status = getaddrinfo(NULL, UDPPORT, &hints, &serverinfo)) != 0) {
      cerr<<"getaddrinfo: \n" << gai_strerror(status);
        return ;
    }
	
    // #Beej :: loop through all the results and bind to the first we can
    for(p = serverinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
	   p->ai_protocol)) == -1) {
	  perror("Server1 listener: socket");
            continue;
        }
		
	// #Beej :: bind usage
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Server1 listener :: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
      cerr << "Server1 listener :: failed to bind socket\n";
      return ;
    }

    freeaddrinfo(serverinfo);
	
	//below code is to find own port
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
      perror("getsockname");
    
      inet_ntop(sin.sin_family, &sin.sin_addr, selfIP, sizeof selfIP);


   
	while(1)
	{
   // #Beej :: recvfrom usage
    addr_len = sizeof remote_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&remote_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    inet_ntop(remote_addr.ss_family,get_in_addr((struct sockaddr *)&remote_addr),remoteIP, sizeof remoteIP);
    int remotePort = ntohs(get_in_port((struct sockaddr *)&remote_addr));
   
    
    buf[numbytes] = '\0';
    strcpy(bufCopy, buf);
    convertMessageToKeyValue(bufCopy, bufKey);
    
    if(clientID == 1)
    cout << "The Server 1 has received a request with key " << bufKey << " from Client 1 with port number "<< remotePort <<" and IP address " << remoteIP << ". \n";
    else
     cout << "The Server 1 has received a request with key " << bufKey << " from Client 2 with port number "<< remotePort <<" and IP address " << remoteIP << ". \n"; 
    int l_status = processInputRequest(bufKey);
    
    if(l_status == 0)
      {
	//	sendUDPResponseToClient(sockfd,(struct sockaddr *)&remote_addr);
	if(clientID == 1)
       	cout << "The Server 1 sends the reply "<< valueToSend <<" to Client 1 with port number "<< remotePort << " and IP address " << remoteIP << ".\n";
	else
	cout << "The Server 1 sends the reply "<< valueToSend <<" to Client 2  with port number "<< remotePort << " and IP address " << remoteIP << ".\n";  
	const char * value = valueToSend.c_str();
  
	if ((numbytes = sendto(sockfd, value, strlen(value), 0, (struct sockaddr *)&remote_addr, addr_len)) == -1) {
	  perror(" Server1 :: sendto");
	  exit(1);
	}
      }

    else if(l_status == 1)
      {
	cout<<"The Server 1 sends the request " << buf << " to the Server 2. \n";

	int r_status = manageTCPServer1Socket(buf);
	if(r_status == 0)
	  {
	   
	    	    updateLocalDataStructure(buf);
	    //      sendUDPResponseToClient(sockfd,(struct sockaddr *)&remote_addr);
	    const char * value = valueToSend.c_str();
	    if ((numbytes = sendto(sockfd, value, strlen(value), 0, (struct sockaddr *)&remote_addr, addr_len)) == -1) {
	      perror("Server1 :: sendto");
	      exit(1);
	    }
	    if(clientID == 1)
	    cout << "The Server 1 sent reply "<< valueToSend <<" to Client 1 with port number "<< remotePort << " and IP address " << remoteIP << ".\n";
	    else
	    cout << "The Server 1 sent reply "<< valueToSend <<" to Client 2  with port number "<< remotePort << " and IP address " << remoteIP << ".\n";

	   }
      }

   // close(sockfd);  Server1 doesnot close the UDP socket as it should keep listening
    
	if(clientID == 1)
		clientID = 2;
	else if (clientID == 2)
		clientID = 1;
		
 }// end of while
}


// This api manages the TCP connection between Server1 and Server2
int  manageTCPServer1Socket(char keyBuf[])
{
    int sockfd, numbytes;  
    char buf[MAXBUFLEN], bufCopy[MAXBUFLEN], bufValue[MAXBUFLEN];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN], selfIP[INET6_ADDRSTRLEN];

    
    const char* keyData = keyBuf;
    // cout << "Server1 :: Received key is "<< keyData <<endl;
    /*    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
	}*/

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	// #Beej :: getaddrinfo usage
    if ((rv = getaddrinfo(SERVERNAME, TCPDESTINATIONPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // #Beej :: loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
		
		
	// #Beej :: connect usage
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

 break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    //    printf("Server1 :: connecting to %s\n", s);

    //below code is to find own port
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
      perror("getsockname");
    
      inet_ntop(sin.sin_family, &sin.sin_addr, selfIP, sizeof selfIP);
      cout<<"The TCP port number is " << ntohs(sin.sin_port) <<" and the IP address is " << selfIP<< ".\n";

    freeaddrinfo(servinfo); // all done with this structure

	// #Beej :: send usage
    if (send(sockfd, keyData, strlen(keyData), 0) == -1){
           perror("send");
           exit(1);
    }

	// #Beej :: recv usage
    if ((numbytes = recv(sockfd, buf, MAXBUFLEN-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
    strcpy(bufCopy, buf);
    convertMessageToKeyValue(bufCopy, bufValue);

    std::string tempS(buf);
    valueToSend = tempS;
     char ip[100];
     convertHostToIP(ip);
     //printf("client: received '%s'\n",buf);

    cout << "The Server 1 received the value "<<bufValue << " from the Server 2 with port number " << TCPDESTINATIONPORT << " and IP address " << ip << ". \n";
    close(sockfd);
    cout <<"The Server 1 closed the TCP connection with the Server 2.\n";	
  
    return 0;
}
