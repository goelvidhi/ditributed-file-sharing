/*
 * dhtserver.h
 *
 *  Created on: Apr 4, 2015
 *      Author: Vidhi Goel
 */

#ifndef DHTSERVER_H_
#define DHTSERVER_H_

void readFromFile();

void manageTCPServer2Socket();
void manageTCPServer3Socket();
void manageUDPServer1Socket();
int manageTCPServer1Socket(char []);
int manageTCPServer2ClientSocket(char []);


#endif /* DHTSERVER_H_ */
