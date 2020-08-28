#pragma once
#include <winsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string> 
#include <fstream>
#include <vector>
#include <windows.h>

#pragma comment(lib,"Wsock32.lib")

using namespace std;

class Network
{

private:

	sockaddr_in addr;
	sockaddr_in new_addr;  

public:

	Network();
	
	SOCKET sock;
	bool is_init = false;    // Do socket init
	bool is_connect = false; // Do socket connected(user socket)
	string dir_downloads = "Download/"; // Directory for load file 

	/* Connected to server, if success - return true */
	bool _connect(const char *address, int port);

	/* Bind socket server, if success - return true */
	bool _bind(const char *address, int port);  

	/* Wait new connect, if new connected when return - object new socket */
    SOCKET _listen();         

	/* Send string data */
	void send_str(const char* message, SOCKET* sock);

	/* Recive file data, defolt size buffer - 1024 byte */
	bool recive_file_data(SOCKET *sock);

	/* Send file data, defolt size buffer - 1024 byte */
	bool send_file_data(string path_to_file, SOCKET *new_sock); 

	/* Send all file of directory, defolt size buffer - 1024 byte */
	//bool send_drirectory_data(const char* path, SOCKET* new_sock);
	 

};