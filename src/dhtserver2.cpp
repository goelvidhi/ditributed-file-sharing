//============================================================================
// Name        : dhtserver2.cpp
// Author      : Vidhi Goel
// Version     :
// Copyright   : Your copyright notice
// Description : Server2 in C++ Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fstream>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "dhtserver.h"

# define SERVERNAME "nunki.usc.edu"
# define SERVERPORT "22024"
# define DESTINATIONPORT "23024"
# define BACKLOG 2
# define MAXBUFLEN 100

using namespace std;

string valueToSend;
string key[12], value[12];
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
   
  if ((rv = getaddrinfo(SERVERNAME, SERVERPORT, &hints, &servinfo)) != 0)
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
       cout << "The Server 2 has TCP port number " << SERVERPORT << " and IP address " << ip << " ." << endl; 
	readFromFile();
	manageTCPServer2Socket();
	return 0;
}

// This api reads the contents of the file and stores them into array of string
void readFromFile()
{
	char * inputFilePath = "../files/server2.txt";
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

	for(int j=0;j<4;j++)
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


// This api takes input key and compares it with stored array to find out the matching value. It returns 0 if success, else returns failure 
int processInputRequest(char *bufKey)
{
  int i;

  for (i = 0; i <= endIndex; i++)
    {
      const char* keyChar = key[i].c_str();
      if(strcmp(bufKey,keyChar) != 0)
	continue;
      //        cout <<"Match found for the input string with value : " << value[i] << endl;
      valueToSend = "POST " +  value[i];
      return 0;
    }
  
  if(i > endIndex)
    {
      //      cout <<"Match not found at Server2. Please hold on, will try at Server 3.";
      return 1;
    }
}


// This api updates the cache of Server2 (string array) whenever it receives a new key-value mapping from Server3
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

// This api handles the TCP connection between Server2 and Server1 to receive/send the requests and response for key/value 
void manageTCPServer2Socket()
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
	
	// #Beej:: getaddrinfo usage
  if((status = getaddrinfo(SERVERNAME,SERVERPORT, &hints, &serverInfo))!= 0){
    cerr <<"getaddrinfo error: ", gai_strerror(status);
    exit(1);
  }

  // #Beej :: socket creation usage
  for(p = serverInfo; p != NULL; p = p->ai_next)
    {
      if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))== -1){
	cerr <<"Server2 :: Socket Failure";
	continue;
      }
    
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
	cerr<<"setsockopt failed";
            exit(1);
        }

		// #Beej :: bind usage
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;	
    }

  if (p == NULL)  {
    cerr << "Server2 :: failed to bind"<< endl;
        return ;
    }

  freeaddrinfo(serverInfo);
  
  // #Beej :: listen usage
  if (listen(sockfd, BACKLOG) == -1) {
    cerr << "listen :: backlog has reached an end" <<endl ;
        exit(1);
    }

  //printf("Server2: waiting for connections...\n");

    while(1) {  // #Beej :: accept() loop
        sin_size = sizeof remote_addr;
        child_fd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
        if (child_fd == -1) {
	  cerr<<"accept :: failed to create child socket";
            continue;
        }

	inet_ntop(remote_addr.ss_family,get_in_addr((struct sockaddr *)&remote_addr),remoteIP, sizeof remoteIP);
        int remotePort = ntohs(get_in_port((struct sockaddr *)&remote_addr));
	
	    
		// #Beej :: recv usage
	    if ((numbytes = recv(child_fd, buf, MAXBUFLEN-1, 0)) == -1) {
	      perror("recv");
	      exit(1);
	    }

	    buf[numbytes] = '\0';
	    strcpy(bufCopy, buf);
	    convertMessageToKeyValue(bufCopy, bufKey);
	    cout<<"The Server 2 has received a request with key " <<bufKey << " from Server 1 with port number " <<remotePort << " and IP address " <<remoteIP << ". \n";
	    
	    int l_status = processInputRequest(bufKey);
	    
    if(l_status == 0)
      {
	//	sendUDPResponseToClient(sockfd,(struct sockaddr *)&remote_addr);
       
	cout << "The Server 2 sends the reply "<< valueToSend <<" to the Server 1 with port number "<< remotePort << " and IP address " << remoteIP << ".\n";
	const char * value = valueToSend.c_str();
  
	if ((numbytes = send(child_fd, value, strlen(value), 0)) == -1) {
	  perror(" Server2 :: sendto");
	  exit(1);
	}
       
      }

    else if(l_status == 1)
      {
       
	cout<<"The Server 2 sends the request " << buf << " to the Server 3.\n";
	int r_status = manageTCPServer2ClientSocket(buf);
	if(r_status == 0)
	  {
	    updateLocalDataStructure(buf);
	    
	    const char * value = valueToSend.c_str();
	    if ((numbytes = send(child_fd, value, strlen(value), 0)) == -1) {
	      perror("Server2 :: sendto");
	      exit(1);
	    }
	     cout << "The Server 2 sends the reply "<< valueToSend <<" to the Server 1 with port number "<< remotePort << " and IP address " << remoteIP << ".\n";
	  }
      }
	    
        close(child_fd);  // parent doesn't need this
    }
	return;

}


// This api handles the TCP connection between Server2 and Server3
int manageTCPServer2ClientSocket(char keyBuf[])
{

int sockfd, numbytes;  
 char buf[MAXBUFLEN], bufCopy[MAXBUFLEN], bufValue[MAXBUFLEN];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN], selfIP[INET6_ADDRSTRLEN];
    
    const char* keyData = keyBuf;
    // cout << "Server1 :: Received key is "<< keyData <<endl;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

	// #Beej :: getaddrinfo usage
    if ((rv = getaddrinfo(SERVERNAME, DESTINATIONPORT, &hints, &servinfo)) != 0) {
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
    // printf("Server2-Client :: connecting to %s\n", s);

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


   
        cout << "The Server 2 received the value "<<bufValue << " from the Server 3 with port number " << DESTINATIONPORT << " and IP address " << ip << ". \n";
    close(sockfd);
    cout <<"The Server 2 closed the TCP connection with the Server 3.\n";	

  

    return 0;
}

    
